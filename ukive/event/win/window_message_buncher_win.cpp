// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "window_message_buncher_win.h"

#include "utils/multi_callbacks.hpp"


namespace ukive {

    WindowMessageBuncherWin::WindowMessageBuncherWin() {
    }

    bool WindowMessageBuncherWin::notifyPowerBroadcast(HWND hWnd) {
        if (!canNotify(hWnd)) {
            return false;
        }
        cur_source_ = hWnd;

        for (auto l : listeners_) {
            l->onWMBPowerBroadcast();
        }
        return true;
    }

    bool WindowMessageBuncherWin::notifyDisplayChanged(HWND hWnd) {
        if (!canNotify(hWnd)) {
            return false;
        }
        cur_source_ = hWnd;

        for (auto l : listeners_) {
            l->onWMBDisplayChanged();
        }
        return true;
    }

    bool WindowMessageBuncherWin::notifySettingChanged(HWND hWnd) {
        if (!canNotify(hWnd)) {
            return false;
        }
        cur_source_ = hWnd;

        for (auto l : listeners_) {
            l->onWMBSettingChanged();
        }
        return true;
    }

    bool WindowMessageBuncherWin::canNotify(HWND hWnd) const {
        if (!hWnd) {
            return false;
        }
        if (cur_source_ && cur_source_ != hWnd) {
            return false;
        }

        return true;
    }

    void WindowMessageBuncherWin::addListener(WindowMessageBuncherListener* l) {
        utl::addCallbackTo(listeners_, l);
    }

    void WindowMessageBuncherWin::removeListener(WindowMessageBuncherListener* l) {
        utl::removeCallbackFrom(listeners_, l);
    }

}
