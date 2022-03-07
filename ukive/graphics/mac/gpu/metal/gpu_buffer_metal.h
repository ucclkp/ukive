// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_BUFFER_METAL_H_
#define UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_BUFFER_METAL_H_

#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gref_count_impl.h"


namespace ukive {
namespace mac {

    class GPUBufferMetal :
        public GPUBuffer,
        public GRefCountImpl
    {
    public:
        GPUBufferMetal(const Desc& desc);

        const Desc& getDesc() const override;

    private:
        Desc desc_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_BUFFER_METAL_H_