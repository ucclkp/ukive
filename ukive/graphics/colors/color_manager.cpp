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
#include "ukive/graphics/mac/colors/color_manager_mac.h"
#endif


namespace ukive {

    // static
    ColorManager* ColorManager::create() {
#ifdef OS_WINDOWS
        return new win::ColorManagerWin();
#elif defined OS_MAC
        return new mac::ColorManagerMac();
#endif
    }

    // static
    bool ColorManager::getDefaultProfile(std::u16string* path) {
#ifdef OS_WINDOWS
        std::wstring w_path;
        bool ret = win::ColorManagerWin::getDefaultProfile(&w_path);
        if (ret) {
            path->assign(w_path.begin(), w_path.end());
        }
        return ret;
#elif defined OS_MAC
        return mac::ColorManagerMac::getDefaultProfile(path);
#endif
    }

}
