// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_SAMPLER_STATE_H_
#define UKIVE_GRAPHICS_GPU_GPU_SAMPLER_STATE_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUSamplerState {
    public:
        using Native = intptr_t;

        enum class Filter {
            MinMagMipPoint,
            MinMagPointMipLinear,
        };

        enum class TextureAddrMode {
            Wrap,
            Mirror,
            Clamp,
            Border,
            MirrorOnce,
        };

        struct Desc {
            Desc()
                : filter(Filter::MinMagPointMipLinear),
                  addr_u(TextureAddrMode::Wrap),
                  addr_v(TextureAddrMode::Wrap),
                  addr_w(TextureAddrMode::Wrap),
                  mip_lod_bias(0),
                  max_anisotropy(0),
                  comp_func(ComparisonFunc::Always),
                  border_color{ 0 },
                  min_lod(0),
                  max_lod(0) {}

            Filter filter;
            TextureAddrMode addr_u;
            TextureAddrMode addr_v;
            TextureAddrMode addr_w;
            float mip_lod_bias;
            uint32_t max_anisotropy;
            ComparisonFunc comp_func;
            float border_color[4];
            float min_lod;
            float max_lod;
        };

        virtual ~GPUSamplerState() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_SAMPLER_STATE_H_