// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_EFFECTS_SHADOW_EFFECT_H_
#define UKIVE_GRAPHICS_EFFECTS_SHADOW_EFFECT_H_

#include "ukive/graphics/effects/cyro_effect.h"


namespace ukive {

    class Context;

    class ShadowEffect : public CyroEffect {
    public:
        static ShadowEffect* create(Context context);

        virtual void resetCache() = 0;
        virtual bool hasCache() const = 0;

        virtual bool setRadius(int radius) = 0;
        virtual int getRadius() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_EFFECTS_SHADOW_EFFECT_H_