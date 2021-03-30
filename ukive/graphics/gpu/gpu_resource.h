// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_RESOURCE_H_
#define UKIVE_GRAPHICS_GPU_GPU_RESOURCE_H_


namespace ukive {

    class GPUResource {
    public:
        enum ResourceType {
            RES_VERTEX_BUFFER   = 1 << 0,
            RES_INDEX_BUFFER    = 1 << 1,
            RES_CONSTANT_BUFFER = 1 << 2,
            RES_SHADER_RES      = 1 << 3,
            RES_RENDER_TARGET   = 1 << 4,
            RES_DEPTH_STENCIL   = 1 << 5,
        };

        enum CPUAccessFlags {
            CPU_ACCESS_READ = 1 << 0,
            CPU_ACCESS_WRITE = 1 << 1,
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