// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_EFFECTS_IMAGE_EFFECT_H_
#define UKIVE_GRAPHICS_EFFECTS_IMAGE_EFFECT_H_

#include "ukive/graphics/effects/cyro_effect.h"
#include "ukive/graphics/size.hpp"


namespace ukive {

    class Context;

    class ImageEffect : public CyroEffect {
    public:
        static ImageEffect* create(Context context);
    };

}

#endif  // UKIVE_GRAPHICS_EFFECTS_IMAGE_EFFECT_H_