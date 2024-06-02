// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_RESOURCE_H_
#define UKIVE_GRAPHICS_GPU_GPU_RESOURCE_H_

#include "ukive/graphics/gref_count.h"


namespace ukive {

    class GPUResource : public virtual GRefCount {
    public:
        enum ResourceType {
            RES_VERTEX_BUFFER   = 1u << 0,
            RES_INDEX_BUFFER    = 1u << 1,
            RES_CONSTANT_BUFFER = 1u << 2,
            RES_SHADER_RES      = 1u << 3,
            RES_RENDER_TARGET   = 1u << 4,
            RES_DEPTH_STENCIL   = 1u << 5,
        };

        enum CPUAccessFlags {
            CPU_ACCESS_NONE  = 0u,
            CPU_ACCESS_READ  = 1u << 0,
            CPU_ACCESS_WRITE = 1u << 1,
        };

        enum MiscFlags {
            RES_MISC_NONE       = 0,
            RES_MISC_GEN_MIPS   = 1u << 0,
            RES_MISC_SHARED     = 1u << 1,
            RES_MISC_TEXCUBE    = 1u << 2,
            RES_MISC_GDI_COMPAT = 1u << 3, // Windows only
        };

        enum class Type {
            Buffer,
            Texture,
        };

        virtual ~GPUResource() = default;

        virtual Type getType() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_RESOURCE_H_