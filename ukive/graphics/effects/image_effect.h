// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_EFFECTS_IMAGE_EFFECT_H_
#define UKIVE_GRAPHICS_EFFECTS_IMAGE_EFFECT_H_

#include <functional>
#include <string>

#include "ukive/graphics/effects/cyro_effect.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/size.hpp"


namespace ukive {

    class GPUTexture;

    class ImageEffect : public CyroEffect {
    public:
        using ParameterUpdateHandler = std::function<void(void*)>;

        static ImageEffect* create();

        virtual bool addInput(const GPtr<GPUTexture>& texture) = 0;
        virtual bool setPixelShader(const std::u16string& name) = 0;
        virtual bool setParameterSize(uint32_t size) = 0;
        virtual void setParameterUpdateHandler(const ParameterUpdateHandler& h) = 0;
        virtual bool updateParameters() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_EFFECTS_IMAGE_EFFECT_H_