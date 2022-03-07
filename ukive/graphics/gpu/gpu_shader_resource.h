// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_SHADER_RESOURCE_H_
#define UKIVE_GRAPHICS_GPU_GPU_SHADER_RESOURCE_H_

#include <cstdint>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUShaderResource : public virtual GRefCount {
    public:
        enum SRVDimension {
            SRV_DIMENSION_UNKNOWN,
            SRV_DIMENSION_BUFFER,
            SRV_DIMENSION_TEXTURE1D,
            SRV_DIMENSION_TEXTURE1DARRAY,
            SRV_DIMENSION_TEXTURE2D,
            SRV_DIMENSION_TEXTURE2DARRAY,
            SRV_DIMENSION_TEXTURE2DMS,
            SRV_DIMENSION_TEXTURE2DMSARRAY,
            SRV_DIMENSION_TEXTURE3D,
            SRV_DIMENSION_TEXTURECUBE,
            SRV_DIMENSION_TEXTURECUBEARRAY,
            SRV_DIMENSION_BUFFEREX,
        };

        struct BufferSRV {
            union {
                uint32_t first_element;
                uint32_t element_offset;
            };
            union {
                uint32_t element_num;
                uint32_t element_width;
            };
        };

        struct Tex1DSRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
        };

        struct Tex1DArraySRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
            uint32_t first_array_slice;
            uint32_t array_size;
        };

        struct Tex2DSRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
        };

        struct Tex2DArraySRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
            uint32_t first_array_slice;
            uint32_t array_size;
        };

        struct Tex2DMSSRV {
        };

        struct Tex2DMSArraySRV {
            uint32_t first_array_slice;
            uint32_t array_size;
        };

        struct Tex3DSRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
        };

        struct TexCubeSRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
        };

        struct TexCubeArraySRV {
            uint32_t most_detailed_mip;
            uint32_t mip_levels;
            uint32_t first_2d_array_face;
            uint32_t cube_num;
        };

        struct BufferExSRV {
            uint32_t first_element;
            uint32_t element_num;
            uint32_t flags;
        };

        struct Desc {
            GPUDataFormat format;
            SRVDimension view_dim;

            BufferSRV buffer;
            Tex1DSRV tex1d;
            Tex1DArraySRV tex1d_array;
            Tex2DSRV tex2d;
            Tex2DArraySRV tex2d_array;
            Tex2DMSSRV tex2dms;
            Tex2DMSArraySRV tex2dms_array;
            Tex3DSRV tex3d;
            TexCubeSRV tex_cube;
            TexCubeArraySRV tex_cube_array;
            BufferExSRV buffer_ex;
        };

        using Native = intptr_t;

        virtual ~GPUShaderResource() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_SHADER_RESOURCE_H_