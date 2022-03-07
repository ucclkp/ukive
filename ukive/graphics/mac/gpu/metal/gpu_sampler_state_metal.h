// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_SAMPLER_STATE_METAL_H_
#define UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_SAMPLER_STATE_METAL_H_

#include "ukive/graphics/gpu/gpu_sampler_state.h"
#include "ukive/graphics/gref_count_impl.h"


namespace ukive {
namespace mac {

    class GPUSamplerStateMetal :
        public GPUSamplerState,
        public GRefCountImpl
    {
    public:
        explicit GPUSamplerStateMetal();

    private:
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_SAMPLER_STATE_METAL_H_