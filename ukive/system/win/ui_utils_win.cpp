// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ui_utils_win.h"

#include <dwmapi.h>
#include <ShellScalingAPI.h>
#include <VersionHelpers.h>
#include <slpublic.h>

#include "ukive/system/win/dynamic_windows_api.h"
#include "ukive/system/ui_utils.h"


namespace ukive {
namespace win {

    bool isActivated() {
        static bool is_vista = ::IsWindowsVistaOrGreater();
        if (is_vista) {
            // https://docs.microsoft.com/en-us/windows/win32/api/slpublic/nf-slpublic-slisgenuinelocal
            GUID app_id;
            RPC_STATUS ret = ::UuidFromStringW(RPC_WSTR(L"55c92734-d682-4d71-983e-d6ec3f16059f"), &app_id);
            if (ret != RPC_S_OK) {
                return false;
            }

            SL_GENUINE_STATE state;
            HRESULT hr = ::SLIsGenuineLocal(&app_id, &state, nullptr);
            if (FAILED(hr)) {
                return false;
            }

            return state == SL_GEN_STATE_IS_GENUINE;
        } else {
            // https://docs.microsoft.com/zh-cn/previous-versions/windows/desktop/wingen/legitcheck
            HMODULE mod = SecureSysLoadLibrary(L"LegitLib.dll");
            if (!mod) {
                return false;
            }

            bool is_genuine;
            typedef int (WINAPI *LegitCheck)();
            LegitCheck f = LegitCheck(::GetProcAddress(mod, "LegitCheck"));
            if (f) {
                is_genuine = f() == 0;
            } else {
                is_genuine = false;
            }
            ::FreeLibrary(mod);
            return is_genuine;
        }
    }

    bool isAeroEnabled() {
        BOOL dwm_enabled = FALSE;
        HRESULT hr = ::DwmIsCompositionEnabled(&dwm_enabled);
        return (SUCCEEDED(hr) && dwm_enabled == TRUE);
    }

    HMODULE SecureSysLoadLibrary(LPCWSTR name) {
        // Safe
        HMODULE mod = ::LoadLibraryExW(
            name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (mod) {
            return mod;
        }

        if (!name) {
            return nullptr;
        }

        size_t name_len = wcslen(name);
        if (name_len == 0) {
            return nullptr;
        }


        WCHAR* path;
        bool is_dynamic = false;

        WCHAR sbuf[MAX_PATH];
        UINT len = ::GetSystemDirectoryW(sbuf, MAX_PATH);
        if (len != 0) {
            size_t d_size = len + name_len + 1;
            if (d_size > MAX_PATH) {
                // buffer 不够
                WCHAR* buf = new WCHAR[d_size];
                UINT new_len = ::GetSystemDirectoryW(buf, len);
                if (new_len != 0 && new_len <= len) {
                    wcscat_s(buf, d_size, name);
                    path = buf;
                    is_dynamic = true;
                } else {
                    delete[] buf;
                    path = nullptr;
                }
            } else {
                wcscat_s(sbuf, MAX_PATH, name);
                path = sbuf;
            }
        } else {
            path = nullptr;
        }

        // Safe
        if (path) {
            mod = ::LoadLibraryW(path);
            if (is_dynamic) {
                delete[] path;
            }
            return mod;
        }

        // Reduce risk
        ::SetDllDirectoryW(L"");
        return ::LoadLibraryW(name);
    }

}

    unsigned int getDoubleClickTime() {
        return ::GetDoubleClickTime();
    }

    Size getCurrentCursorSize() {
        // https://stackoverflow.com/questions/1699666/how-do-i-know-the-size-of-a-hcursor-object
        Size r(0, 0);
        auto cur = ::GetCursor();
        if (!cur) {
            return r;
        }

        ICONINFO info;
        if (::GetIconInfo(cur, &info) == 0) {
            return r;
        }

        BITMAP bmp_info;
        if (::GetObjectW(info.hbmMask, sizeof(BITMAP), &bmp_info) != 0) {
            r.width(bmp_info.bmWidth);
            r.height(std::abs(bmp_info.bmHeight) / ((!info.hbmColor) ? 2 : 1));
        }

        if (info.hbmColor) {
            ::DeleteObject(info.hbmColor);
        }
        ::DeleteObject(info.hbmMask);

        return r;
    }

}
