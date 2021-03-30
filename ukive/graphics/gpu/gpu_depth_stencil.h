// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_DEPTH_STENCIL_H_
#define UKIVE_GRAPHICS_GPU_GPU_DEPTH_STENCIL_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUDepthStencil {
    public:
        enum DSVDimension {
            DSV_DIMENSION_UNKNOWN,
            DSV_DIMENSION_TEXTURE1D,
            DSV_DIMENSION_TEXTURE1DARRAY,
            DSV_DIMENSION_TEXTURE2D,
            DSV_DIMENSION_TEXTURE2DARRAY,
            DSV_DIMENSION_TEXTURE2DMS,
            DSV_DIMENSION_TEXTURE2DMSARRAY,
        };

        struct Tex1DDSV {
            uint32_t mip_slice = 0;
        };

        struct Tex1DArrayDSV {
            uint32_t mip_slice = 0;
            uint32_t first_array_slice = 0;
            uint32_t array_size = 0;
        };

        struct Tex2DDSV {
            uint32_t mip_slice = 0;
        };

        struct Tex2DArrayDSV {
            uint32_t mip_slice = 0;
            uint32_t first_array_slice = 0;
            uint32_t array_size = 0;
        };

        struct Tex2DMSDSV {
            uint32_t unused = 0;
        };

        struct Tex2DMSArrayDSV {
            uint32_t first_array_slice = 0;
            uint32_t array_size = 0;
        };

        struct Desc {
            Desc()
                : format(GPUDataFormat::UNKNOWN),
                  view_dim(DSV_DIMENSION_UNKNOWN),
                  flags(0) {}

            GPUDataFormat format;
            DSVDimension view_dim;
            uint32_t flags;

            Tex1DDSV tex1d;
            Tex1DArrayDSV tex1d_array;
            Tex2DDSV tex2d;
            Tex2DArrayDSV tex2d_array;
            Tex2DMSDSV tex2dms;
            Tex2DMSArrayDSV tex2dms_array;
        };

        virtual ~GPUDepthStencil() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_DEPTH_STENCIL_H_