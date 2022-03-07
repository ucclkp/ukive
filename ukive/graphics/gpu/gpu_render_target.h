// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_RENDER_TARGET_H_
#define UKIVE_GRAPHICS_GPU_GPU_RENDER_TARGET_H_

#include <cstdint>

#include "ukive/graphics/gref_count.h"


namespace ukive {

    class GPURenderTarget : public virtual GRefCount {
    public:
        using Native = intptr_t;

        virtual ~GPURenderTarget() = default;

        virtual Native getOpaque() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_RENDER_TARGET_H_