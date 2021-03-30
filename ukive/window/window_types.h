// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_TYPES_H_
#define UKIVE_WINDOW_WINDOW_TYPES_H_


namespace ukive {

    enum WindowFrameType {
        WINDOW_FRAME_NATIVE,
        WINDOW_FRAME_CUSTOM,
    };

    enum WindowResizeType {
        WINDOW_RESIZE_RESTORED,
        WINDOW_RESIZE_MINIMIZED,
        WINDOW_RESIZE_MAXIMIZED,
    };

    enum WindowCloseMethod {
        WINDOW_CLOSE_BY_BUTTON = 0,
        WINDOW_CLOSE_BY_MENU = 1 << 0,
        WINDOW_CLOSE_BY_ESC = 1 << 1,
    };

}

#endif  // UKIVE_WINDOW_WINDOW_TYPES_H_