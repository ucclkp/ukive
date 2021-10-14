// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "display_manager_win.h"

#include "utils/log.h"

#include "ukive/graphics/win/display_win.h"
#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"


namespace ukive {

    DisplayManagerWin::DisplayManagerWin() {
        null_display_ = std::make_shared<DisplayWin>(nullptr);
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromNull() {
        return null_display_;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromPrimary() {
        POINT pt = { 0, 0 };
        HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return {};
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromPoint(const Point& p) {
        POINT pt{ p.x, p.y };
        HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return {};
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromRect(const Rect& r) {
        RECT win_rect{ r.left, r.top, r.right, r.bottom };
        HMONITOR monitor = ::MonitorFromRect(&win_rect, MONITOR_DEFAULTTONEAREST);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return {};
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromWindow(const Window* w) {
        if (!w) {
            return {};
        }
        auto win = static_cast<WindowImplWin*>(w->getImpl());
        if (!win) {
            return {};
        }

        return fromWindowImpl(win);
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromWindowImpl(const WindowImplWin* win) {
        HMONITOR monitor;
        if (win->isCreated()) {
            monitor = ::MonitorFromWindow(win->getHandle(), MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                LOG(Log::WARNING) << "Failed to get monitor handle!";
                return {};
            }
        } else {
            auto bounds = win->getBounds();
            RECT win_rect;
            win_rect.left = bounds.left;
            win_rect.top = bounds.top;
            win_rect.right = bounds.right;
            win_rect.bottom = bounds.bottom;
            monitor = ::MonitorFromRect(&win_rect, MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                LOG(Log::WARNING) << "Failed to get monitor handle!";
                return {};
            }
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    const DisplayManager::DisplayList& DisplayManagerWin::getAllDisplays() const {
        return list_;
    }

    void DisplayManagerWin::notifyChanged(HWND hWnd) {
        if (!hWnd) {
            return;
        }
        if (cur_source_ && cur_source_ != hWnd) {
            return;
        }
        cur_source_ = hWnd;

        for (auto l : listeners_) {
            l->onDisplayChanged();
        }
    }

    DisplayManager::DisplayPtr DisplayManagerWin::findMonitor(HMONITOR monitor) const {
        for (auto ptr : list_) {
            if (static_cast<DisplayWin*>(ptr.get())->getNative() == monitor) {
                return ptr;
            }
        }
        return {};
    }

}
