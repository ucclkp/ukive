// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "win_app_bar.h"

#include <windows.h>
#include <shellapi.h>
#include <VersionHelpers.h>

#include "ukive/graphics/display.h"
#include "ukive/system/win/win10_version.h"


namespace ukive {
namespace win {

    // static
    bool WinAppBar::isAutoHide() {
        APPBARDATA data;
        data.cbSize = sizeof(APPBARDATA);
        auto ret = ::SHAppBarMessage(ABM_GETSTATE, &data);
        switch (ret) {
        case 0:
            break;
        case ABS_ALWAYSONTOP:
            break;
        case ABS_AUTOHIDE:
            return true;
        default:
            break;
        }
        return false;
    }

    // static
    bool WinAppBar::hasAutoHideBar(Display* display) {
        auto bounds = display->getPixelBounds();
        if (bounds != display->getPixelWorkArea()) {
            return false;
        }
        return true;
    }

    // static
    WinAppBar::Edge WinAppBar::findAutoHideEdge(Display* display) {
        if (!hasAutoHideBar(display)) {
            return None;
        }

        DWORD msg;
        APPBARDATA data;
        data.cbSize = sizeof(APPBARDATA);
        data.uEdge = ABE_BOTTOM;

        if (::IsWindows8OrGreater()) {
            msg = ABM_GETAUTOHIDEBAREX;

            auto bounds = display->getPixelBounds();
            data.rc.left = bounds.left;
            data.rc.top = bounds.top;
            data.rc.right = bounds.right;
            data.rc.bottom = bounds.bottom;
        } else {
            /**
             * Windows 7 只有主显示器才会显示 AppBar，且使用
             * ABM_GETAUTOHIDEBAREX 什么也拿不到。
             */
            if (!Display::fromPrimary()->isSameDisplay(display)) {
                return None;
            }

            msg = ABM_GETAUTOHIDEBAR;

            data.rc.left = 0;
            data.rc.top = 0;
            data.rc.right = 0;
            data.rc.bottom = 0;
        }

        // https://docs.microsoft.com/zh-cn/windows/win32/api/shellapi/nf-shellapi-shappbarmessage
        auto handle = ::SHAppBarMessage(msg, &data);
        if (handle) {
            return Bottom;
        }
        data.uEdge = ABE_LEFT;
        handle = ::SHAppBarMessage(msg, &data);
        if (handle) {
            return Left;
        }
        data.uEdge = ABE_RIGHT;
        handle = ::SHAppBarMessage(msg, &data);
        if (handle) {
            return Right;
        }
        data.uEdge = ABE_TOP;
        handle = ::SHAppBarMessage(msg, &data);
        if (handle) {
            return Top;
        }
        return None;
    }

    // static
    int WinAppBar::getAutoHideBarThickness() {
        return 1;
    }

}
}