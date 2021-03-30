// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_REG_MANAGER_H_
#define UKIVE_SYSTEM_WIN_REG_MANAGER_H_

#include <string>


namespace ukive {

    class RegManager {
    public:
        struct SysThemeConfig {
            bool apps_use_light_theme;
            bool system_uses_light_theme;
            bool transparency_enabled;
        };

        static bool registerApplication();
        static bool unregisterApplication();

        static bool registerProgId(const std::wstring& progid, const std::wstring& cmd);
        static bool unregisterProgId(const std::wstring& progid);

        static bool associateExtName(
            const std::wstring& ext_name, const std::wstring& progid);
        static bool unassociateExtName(
            const std::wstring& ext_name, const std::wstring& progid);

        static bool hasProgId(const std::wstring& progid);
        static bool hasProgIdInExt(const std::wstring& ext_name, const std::wstring& progid);

        // 仅在 Windows 10 上可用
        static bool getSysThemeConfig(SysThemeConfig* config);

    private:
        static bool createProgId(const std::wstring& progid, const std::wstring& cmd);
        static bool deleteProgId(const std::wstring& progid);

        static bool addProgIdToExt(const std::wstring& ext_name, const std::wstring& progid);
        static bool removeProgIdFromExt(const std::wstring& ext_name, const std::wstring& progid);
    };

}

#endif  // UKIVE_SYSTEM_WIN_REG_MANAGER_H_