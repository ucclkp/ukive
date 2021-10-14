// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_DISPLAY_MANAGER_WIN_H_
#define UKIVE_GRAPHICS_DISPLAY_MANAGER_WIN_H_

#include "ukive/graphics/display_manager.h"

#include <Windows.h>


namespace ukive {

    class WindowImplWin;

    class DisplayManagerWin : public DisplayManager {
    public:
        DisplayManagerWin();

        DisplayPtr fromNull() override;
        DisplayPtr fromPrimary() override;
        DisplayPtr fromPoint(const Point& p) override;
        DisplayPtr fromRect(const Rect& r) override;
        DisplayPtr fromWindow(const Window* w) override;
        DisplayPtr fromWindowImpl(const WindowImplWin* win);

        const DisplayList& getAllDisplays() const override;

        void notifyChanged(HWND hWnd);

    private:
        DisplayPtr findMonitor(HMONITOR monitor) const;

        DisplayPtr null_display_;
        DisplayList list_;
        HWND cur_source_ = nullptr;
    };

}

#endif  // UKIVE_GRAPHICS_DISPLAY_MANAGER_WIN_H_