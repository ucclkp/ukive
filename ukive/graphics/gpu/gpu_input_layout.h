// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_INPUT_LAYOUT_H_
#define UKIVE_GRAPHICS_GPU_GPU_INPUT_LAYOUT_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUInputLayout {
    public:
        enum AlignedByteOffset : uint32_t {
            Append,
        };

        struct Desc {
            const char* name;
            uint32_t index;
            uint32_t input_slot;
            uint32_t aligned_byte_offset;
            GPUDataFormat format;

            Desc()
                : name(nullptr),
                  index(0),
                  input_slot(0),
                  aligned_byte_offset(0),
                  format(GPUDataFormat::UNKNOWN) {}

            Desc(
                const char* name, uint32_t index,
                uint32_t input_slot, uint32_t aligned_byte_offset,
                GPUDataFormat format)
                : name(name), index(index),
                  input_slot(input_slot), aligned_byte_offset(aligned_byte_offset),
                  format(format) {}
        };

        virtual ~GPUInputLayout() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_INPUT_LAYOUT_H_