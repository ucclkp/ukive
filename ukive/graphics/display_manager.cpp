// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "display_manager.h"

#include "utils/multi_callbacks.hpp"
#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/display_manager_win.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/display_manager_mac.h"
#endif


namespace ukive {

    // static
    DisplayManager* DisplayManager::create() {
#ifdef OS_WINDOWS
        return new DisplayManagerWin();
#elif defined OS_MAC
        return new DisplayManagerMac();
#endif
    }

    void DisplayManager::addListener(DisplayStatusListener* l) {
        utl::addCallbackTo(listeners_, l);
    }

    void DisplayManager::removeListener(DisplayStatusListener* l) {
        utl::removeCallbackFrom(listeners_, l);
    }

}
