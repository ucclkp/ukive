// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_SHADER_H_
#define UKIVE_GRAPHICS_GPU_GPU_SHADER_H_

#include "ukive/graphics/gref_count.h"


namespace ukive {

    class GPUShader : public virtual GRefCount {
    public:
        virtual ~GPUShader() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_SHADER_H_