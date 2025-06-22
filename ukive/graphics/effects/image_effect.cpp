// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/effects/image_effect.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/effects/image_effect_dx.h"
#elif defined OS_MAC
//#include "ukive/graphics/mac/effects/image_effect_mac.h"
#endif


namespace ukive {

    // static
    ImageEffect* ImageEffect::create() {
#ifdef OS_WINDOWS
        return new win::ImageEffectGPU();
#elif defined OS_MAC
        return nullptr;
#endif
    }

}
