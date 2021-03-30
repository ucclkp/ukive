// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_DEPTH_STENCIL_STATE_H_
#define UKIVE_GRAPHICS_GPU_GPU_DEPTH_STENCIL_STATE_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUDepthStencilState {
    public:
        enum StencilOp {
            Keep,
            Zero,
            Replace,
            IncrSat,
            DecrSat,
            Invert,
            Incr,
            Decr,
        };

        enum class WriteMask {
            Zero,
            All,
        };

        struct StencilOpDesc {
            StencilOpDesc()
                : stencil_fail_op(Keep),
                  stencil_depth_fail_op(Keep),
                  stencil_pass_op(Keep),
                  stencil_func(ComparisonFunc::Always) {}

            StencilOp stencil_fail_op;
            StencilOp stencil_depth_fail_op;
            StencilOp stencil_pass_op;
            ComparisonFunc stencil_func;
        };

        struct Desc {
            Desc()
                : depth_enabled(false),
                  depth_write_mask(WriteMask::Zero),
                  depth_func(ComparisonFunc::Always),
                  stencil_enabled(false),
                  stencil_read_mask(0),
                  stencil_write_mask(0) {}

            bool depth_enabled;
            WriteMask depth_write_mask;
            ComparisonFunc depth_func;
            bool stencil_enabled;
            uint8_t stencil_read_mask;
            uint8_t stencil_write_mask;
            StencilOpDesc front_face;
            StencilOpDesc back_face;
        };

        virtual ~GPUDepthStencilState() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_DEPTH_STENCIL_STATE_H_