// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/system/win/reg_manager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj.h>

#include "utils/log.h"
#include "utils/convert.h"
#include "utils/number.hpp"


namespace ukive {

    bool RegManager::registerApplication() {
        return false;
    }

    bool RegManager::unregisterApplication() {
        return false;
    }

    bool RegManager::registerProgId(const std::wstring& progid, const std::wstring& cmd) {
        if (!createProgId(progid, cmd)) {
            return false;
        }
        return true;
    }

    bool RegManager::unregisterProgId(const std::wstring& progid) {
        if (!deleteProgId(progid)) {
            return false;
        }
        return true;
    }

    bool RegManager::associateExtName(const std::wstring& ext_name, const std::wstring& progid)
    {
        if (!hasProgId(progid)) {
            return false;
        }

        if (!addProgIdToExt(ext_name, progid)) {
            return false;
        }

        ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

        return true;
    }

    bool RegManager::unassociateExtName(const std::wstring& ext_name, const std::wstring& progid) {
        if (!hasProgId(progid)) {
            return false;
        }

        if (!removeProgIdFromExt(ext_name, progid)) {
            return false;
        }

        ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

        return true;
    }

    bool RegManager::createProgId(const std::wstring& progid, const std::wstring& cmd) {
        std::wstring class_path(L"SOFTWARE\\Classes\\");

        // 创建 ProgID
        HKEY progid_key = nullptr;
        std::wstring progid_path = class_path + progid;
        auto ls = ::RegCreateKeyExW(
            HKEY_CURRENT_USER, progid_path.c_str(),
            0, nullptr, 0, KEY_WRITE, nullptr, &progid_key, nullptr);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        std::wstring friendly_name(L"XPicture Image Viewer");
        ls = ::RegSetValueExW(
            progid_key, L"", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(friendly_name.data()),
            utl::num_cast<DWORD>(friendly_name.size()) * sizeof(wchar_t) + 1);
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        // Shell
        HKEY cmd_key = nullptr;
        std::wstring command_path = progid_path + L"\\Shell\\Open\\Command";
        ls = ::RegCreateKeyExW(
            HKEY_CURRENT_USER, command_path.c_str(),
            0, nullptr, 0, KEY_WRITE, nullptr, &cmd_key, nullptr);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegSetValueExW(
            cmd_key, L"", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(cmd.data()),
            utl::num_cast<DWORD>(cmd.size()) * sizeof(wchar_t) + 1);
        result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(cmd_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::deleteProgId(const std::wstring& progid) {
        std::wstring class_path(L"SOFTWARE\\Classes\\");

        // 删除 ProgID
        HKEY progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(HKEY_CURRENT_USER, class_path.c_str(), 0, KEY_ALL_ACCESS, &progid_key);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegDeleteTreeW(progid_key, progid.c_str());
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::hasProgId(const std::wstring& progid) {
        std::wstring progid_path = L"SOFTWARE\\Classes\\" + progid;

        // 尝试打开 ProgID
        HKEY progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(HKEY_CURRENT_USER, progid_path.c_str(), 0, KEY_READ, &progid_key);
        if (ls == ERROR_SUCCESS) {
            return true;
        }
        if (ls == ERROR_FILE_NOT_FOUND) {
            return false;
        } else {
            // TODO:
            return false;
        }
    }

    bool RegManager::addProgIdToExt(const std::wstring& ext_name, const std::wstring& progid) {
        std::wstring class_path = L"SOFTWARE\\Classes\\";
        std::wstring ext_progid_path = class_path + ext_name + L"\\OpenWithProgids";

        // 先在对应扩展名的 OpenWithProgids 子键中创建一个值
        HKEY ext_progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, ext_progid_path.c_str(), 0, KEY_SET_VALUE, &ext_progid_key);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegSetValueExW(
            ext_progid_key,
            progid.c_str(),
            0, REG_SZ, nullptr, 0);
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(ext_progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::removeProgIdFromExt(const std::wstring& ext_name, const std::wstring& progid) {
        std::wstring class_path = L"SOFTWARE\\Classes\\";
        std::wstring ext_progid_path = class_path + ext_name + L"\\OpenWithProgids";

        // 删除对应扩展名的 OpenWithProgids 子键中的值
        HKEY ext_progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, ext_progid_path.c_str(), 0, KEY_ALL_ACCESS, &ext_progid_key);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegDeleteValueW(
            ext_progid_key,
            progid.c_str());
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(ext_progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::hasProgIdInExt(const std::wstring& ext_name, const std::wstring& progid) {
        std::wstring class_path = L"SOFTWARE\\Classes\\";
        std::wstring ext_progid_path = class_path + ext_name + L"\\OpenWithProgids";

        // 打开对应扩展名的 OpenWithProgids 子键
        HKEY ext_progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, ext_progid_path.c_str(), 0, KEY_READ, &ext_progid_key);
        if (ls != ERROR_SUCCESS) {
            if (ls == ERROR_FILE_NOT_FOUND) {
                return false;
            } else {
                // TODO:
                return false;
            }
        }

        ls = ::RegGetValueW(
            ext_progid_key, nullptr, progid.c_str(), RRF_RT_REG_SZ,
            nullptr, nullptr, nullptr);
        if (ls != ERROR_SUCCESS) {
            if (ls == ERROR_FILE_NOT_FOUND) {
                return false;
            } else {
                // TODO:
                return false;
            }
        }

        return true;
    }

    bool RegManager::getSysThemeConfig(SysThemeConfig* config) {
        std::wstring path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

        HKEY pers_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, path.c_str(), 0, KEY_READ, &pers_key);
        if (ls != ERROR_SUCCESS) {
            return false;
        }

        DWORD size = 4;
        DWORD apps_use_lt;
        ls = ::RegGetValueW(
            pers_key, nullptr, L"AppsUseLightTheme", RRF_RT_REG_DWORD,
            nullptr, &apps_use_lt, &size);
        if (ls != ERROR_SUCCESS) {
            LOG(Log::WARNING) << "Cannot read theme info (apps_theme): " << ls;
            return false;
        }

        size = 4;
        DWORD sys_uses_lt;
        ls = ::RegGetValueW(
            pers_key, nullptr, L"SystemUsesLightTheme", RRF_RT_REG_DWORD,
            nullptr, &sys_uses_lt, &size);
        if (ls != ERROR_SUCCESS) {
            LOG(Log::WARNING) << "Cannot read theme info (sys_theme): " << ls;
            return false;
        }

        size = 4;
        DWORD enable_trans;
        ls = ::RegGetValueW(
            pers_key, nullptr, L"EnableTransparency", RRF_RT_REG_DWORD,
            nullptr, &enable_trans, &size);
        if (ls != ERROR_SUCCESS) {
            LOG(Log::WARNING) << "Cannot read theme info (sys_theme): " << ls;
            return false;
        }

        config->apps_use_light_theme = apps_use_lt == 1;
        config->system_uses_light_theme = sys_uses_lt == 1;
        config->transparency_enabled = enable_trans == 1;
        return true;
    }
}