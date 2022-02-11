// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color_manager.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/colors/color_manager_win.h"
#elif defined OS_MAC
#include "ukive/graphics/win/colors/color_manager_mac.h"
#endif


namespace ukive {

    // static
    ColorManager* ColorManager::create() {
#ifdef OS_WINDOWS
        return new win::ColorManagerWin();
#elif defined OS_MAC
#endif
    }


}
