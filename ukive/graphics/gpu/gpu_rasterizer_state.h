// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_RASTERIZER_STATE_H_
#define UKIVE_GRAPHICS_GPU_GPU_RASTERIZER_STATE_H_

#include "ukive/graphics/gref_count.h"


namespace ukive {

    class GPURasterizerState : public virtual GRefCount {
    public:
        enum class FillMode {
            WireFrame,
            Solid,
        };

        enum class CullMode {
            None,
            Front,
            Back,
        };

        struct Desc {
            Desc()
                : fill_mode(FillMode::Solid),
                  cull_mode(CullMode::None),
                  front_counter_clockwise(false),
                  depth_clip_enabled(false),
                  scissor_enabled(false),
                  multisample_enabled(false),
                  antialiased_line_enabled(false) {}

            FillMode fill_mode;
            CullMode cull_mode;
            bool front_counter_clockwise;
            bool depth_clip_enabled;
            bool scissor_enabled;
            bool multisample_enabled;
            bool antialiased_line_enabled;
        };

        virtual ~GPURasterizerState() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_RASTERIZER_STATE_H_