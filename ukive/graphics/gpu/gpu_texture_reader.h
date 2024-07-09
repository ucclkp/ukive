// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_TEXTURE_READER_H_
#define UKIVE_GRAPHICS_GPU_GPU_TEXTURE_READER_H_

#include <cstdint>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gpu/gpu_device.h"
#include "ukive/graphics/images/lc_image_frame.h"


namespace ukive {

    enum TextureReadingFlags {
        TRF_NONE = 0,
        TRF_SRGB = 1u << 0,
        TRF_GEN_MIPMAP = 1u << 1,
    };

    GPtr<GPUTexture> CreateGPUTexture2DFromLcImage(
        const GPtr<GPUDevice>& device,
        const GPtr<LcImageFrame>& source,
        uint32_t flags);

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_TEXTURE_READER_H_