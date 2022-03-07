// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_INPUT_LAYOUT_H_
#define UKIVE_GRAPHICS_GPU_GPU_INPUT_LAYOUT_H_

#include <cstdint>

#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUInputLayout : public virtual GRefCount {
    public:
        struct Desc {
            const char* name;
            uint32_t index;
            uint32_t input_slot;
            uint32_t aligned_byte_offset;
            GPUDataFormat format;
            bool is_align_append;

            Desc()
                : name(nullptr),
                  index(0),
                  input_slot(0),
                  aligned_byte_offset(0),
                  format(GPUDataFormat::UNKNOWN),
                  is_align_append(true) {}

            Desc(
                const char* name,
                GPUDataFormat format,
                uint32_t index = 0,
                uint32_t input_slot = 0,
                bool is_append = true, uint32_t aligned_byte_offset = 0)
                : name(name), index(index),
                  input_slot(input_slot), aligned_byte_offset(aligned_byte_offset),
                  format(format),
                  is_align_append(is_append) {}
        };

        virtual ~GPUInputLayout() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_INPUT_LAYOUT_H_