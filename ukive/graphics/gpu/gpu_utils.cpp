// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_utils.h"

#include <cassert>


namespace ukive {

    GPUDataFormat MapGPUFormat(ImagePixelFormat format) {
        GPUDataFormat gpu_format;
        switch (format) {
        case ImagePixelFormat::RAW:
            gpu_format = GPUDataFormat::UNKNOWN;
            break;
        case ImagePixelFormat::R8G8B8A8_UNORM:
            gpu_format = GPUDataFormat::R8G8B8A8_UNORM;
            break;
        case ImagePixelFormat::B8G8R8A8_UNORM:
            gpu_format = GPUDataFormat::B8G8R8A8_UNORM;
            break;
        case ImagePixelFormat::R16G16B16A16_FLOAT:
            gpu_format = GPUDataFormat::R16G16B16A16_FLOAT;
            break;
        case ImagePixelFormat::R10G10B10A2_UNORM:
            gpu_format = GPUDataFormat::R10G10B10A2_UNORM;
            break;
        default:
            assert(false && "Unknown ImagePixelFormat -> GPUDataFormat!");
            gpu_format = GPUDataFormat::B8G8R8A8_UNORM;
            break;
        }
        return gpu_format;
    }

}