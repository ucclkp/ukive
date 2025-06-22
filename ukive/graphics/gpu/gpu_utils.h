// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_UTILS_H_
#define UKIVE_GRAPHICS_GPU_GPU_UTILS_H_

#include "ukive/graphics/gpu/gpu_types.h"
#include "ukive/graphics/images/image_options.h"


namespace ukive {

    GPUDataFormat MapGPUFormat(ImagePixelFormat format);

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_UTILS_H_