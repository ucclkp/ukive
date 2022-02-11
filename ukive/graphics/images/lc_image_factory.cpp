// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/lc_image_factory.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/images/lc_image_factory_win.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/images/lc_image_factory_mac.h"
#endif


namespace ukive {

    // static
    LcImageFactory* LcImageFactory::create() {
#ifdef OS_WINDOWS
        return new win::LcImageFactoryWin();
#elif defined OS_MAC
        return new LcImageFactoryMac();
#endif
    }

}
