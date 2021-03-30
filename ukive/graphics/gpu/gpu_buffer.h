// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_BUFFER_H_
#define UKIVE_GRAPHICS_GPU_GPU_BUFFER_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_resource.h"


namespace ukive {

    class GPUBuffer : public GPUResource {
    public:
        struct Desc {
            Desc()
                : byte_width(0),
                  res_type(0),
                  struct_byte_stride(0),
                  cpu_access_flag(0),
                  is_dynamic(false) {}

            uint32_t byte_width;
            uint32_t res_type;
            uint32_t struct_byte_stride;
            uint32_t cpu_access_flag;
            bool is_dynamic;
        };

        struct ResourceData {
            ResourceData()
                : sys_mem(nullptr),
                  pitch(0),
                  slice_pitch(0) {}

            const void* sys_mem;
            uint32_t pitch;
            uint32_t slice_pitch;
        };

        Type getType() const override { return Type::Buffer; }

        virtual const Desc& getDesc() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_BUFFER_H_