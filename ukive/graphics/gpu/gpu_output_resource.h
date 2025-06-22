// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_OUTPUT_RESOURCE_H_
#define UKIVE_GRAPHICS_GPU_GPU_OUTPUT_RESOURCE_H_

#include <cstdint>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUOutputResource : public virtual GRefCount {
    public:
        enum UAVDimension {
            UAV_DIMENSION_UNKNOWN,
            UAV_DIMENSION_BUFFER,
            UAV_DIMENSION_TEXTURE1D,
            UAV_DIMENSION_TEXTURE1DARRAY,
            UAV_DIMENSION_TEXTURE2D,
            UAV_DIMENSION_TEXTURE2DARRAY,
            UAV_DIMENSION_TEXTURE3D,
        };

        struct BufferUAV {
            uint32_t first_element;
            uint32_t element_num;
            uint32_t flags;
        };

        struct Tex1DUAV {
            uint32_t mip_slices;
        };

        struct Tex1DArrayUAV {
            uint32_t mip_slices;
            uint32_t first_array_slice;
            uint32_t array_size;
        };

        struct Tex2DUAV {
            uint32_t mip_slices;
        };

        struct Tex2DArrayUAV {
            uint32_t mip_slices;
            uint32_t first_array_slice;
            uint32_t array_size;
        };

        struct Tex3DUAV {
            uint32_t mip_slices;
            uint32_t first_w_slice;
            uint32_t w_size;
        };

        struct Desc {
            GPUDataFormat format;
            UAVDimension view_dim;

            BufferUAV buffer;
            Tex1DUAV tex1d;
            Tex1DArrayUAV tex1d_array;
            Tex2DUAV tex2d;
            Tex2DArrayUAV tex2d_array;
            Tex3DUAV tex3d;
        };

        using Native = intptr_t;

        virtual ~GPUOutputResource() = default;
    };
}

#endif  // UKIVE_GRAPHICS_GPU_GPU_OUTPUT_RESOURCE_H_