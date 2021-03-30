// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/display.h"

#include <memory>

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/display_win.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/display_mac.h"
#endif


namespace ukive {

    thread_local std::unique_ptr<Display> current;

    // static
    Display* Display::create() {
#ifdef OS_WINDOWS
        return new DisplayWin();
#elif defined OS_MAC
        return new DisplayMac();
#endif
    }

    // static
    Display* Display::primary() {
        if (!current) {
            current.reset(create());
        }
        current->makePrimary();
        return current.get();
    }

    // static
    Display* Display::fromWindow(Window* w) {
        if (!current) {
            current.reset(create());
        }
        current->makeFromWindow(w);
        return current.get();
    }

}
