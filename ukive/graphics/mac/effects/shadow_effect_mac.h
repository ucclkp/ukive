// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_EFFECTS_SHADOW_EFFECT_MAC_H_
#define UKIVE_GRAPHICS_MAC_EFFECTS_SHADOW_EFFECT_MAC_H_

#include <memory>

#include "ukive/graphics/effects/shadow_effect.h"

#include "utils/mac/objc_utils.hpp"

UTL_OBJC_CLASS(NSShadow);


namespace ukive {
namespace mac {

    class ShadowEffectMac : public ShadowEffect {
    public:
        ShadowEffectMac();

        bool initialize() override;
        void destroy() override;

        bool generate(Canvas* c) override;
        bool draw(Canvas* c) override;
        bool setContent(OffscreenBuffer* content) override;
        GPtr<ImageFrame> getOutput() const override;

        void resetCache() override;
        bool hasCache() const override;

        bool setRadius(int radius) override;
        int getRadius() const override;

    private:
        int radius_ = 0;
        NSShadow* shadow_ = nullptr;
        GPtr<ImageFrame> content_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_EFFECTS_SHADOW_EFFECT_MAC_H_
