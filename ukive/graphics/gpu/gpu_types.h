// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_TYPES_H_
#define UKIVE_GRAPHICS_GPU_GPU_TYPES_H_


namespace ukive {

    enum class GPUDataFormat {
        UNKNOWN,
        R32G32B32A32_FLOAT,
        R32G32B32A32_UINT,
        R32G32B32_FLOAT,
        R32G32B32_UINT,
        R32G32_FLOAT,
        R16G16B16A16_FLOAT,
        R16G16B16A16_UINT,
        R8G8B8A8_UINT,
        R32_UINT,
        R32_FLOAT,
        R8_UINT,
        B8G8B8R8_UNORM,
        D24_UNORM_S8_UINT,
    };

    enum class ComparisonFunc {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
    };

    struct Viewport {
        Viewport()
            : x(0),
              y(0),
              width(0),
              height(0),
              min_depth(0),
              max_depth(0) {}

        float x;
        float y;
        float width;
        float height;
        float min_depth;
        float max_depth;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_TYPES_H_