// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CURSOR_H_
#define UKIVE_GRAPHICS_CURSOR_H_


namespace ukive {

    enum class Cursor {
        ARROW,
        IBEAM,
        WAIT,
        CROSS,
        UPARROW,
        SIZENWSE,
        SIZENESW,
        SIZEWE,
        SIZENS,
        SIZEALL,
        _NO,
        HAND,
        APPSTARTING,
        HELP,
    };

    enum class HitPoint {
        TOP_LEFT,
        TOP,
        TOP_RIGHT,
        LEFT,
        CLIENT,
        RIGHT,
        BOTTOM_LEFT,
        BOTTOM,
        BOTTOM_RIGHT,
        CAPTION,
        SYS_MENU,
        MIN_BUTTON,
        MAX_BUTTON,
        CLOSE_BUTTON,
    };

}

#endif  // UKIVE_GRAPHICS_CURSOR_H_
