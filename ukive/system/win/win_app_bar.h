// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_WIN_APP_BAR_H_
#define UKIVE_SYSTEM_WIN_WIN_APP_BAR_H_


namespace ukive {

    class Display;

namespace win {

    class WinAppBar {
    public:
        enum Edge {
            None,
            Bottom,
            Left,
            Right,
            Top,
        };

        WinAppBar() = default;

        static bool isAutoHide();
        static bool hasAutoHideBar(Display* display);
        static Edge findAutoHideEdge(Display* display);
        static int getAutoHideBarThickness();
    };

}
}

#endif  // UKIVE_SYSTEM_WIN_WIN_APP_BAR_H_