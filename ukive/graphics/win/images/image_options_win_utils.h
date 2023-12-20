// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGES_IMAGE_OPTIONS_WIN_UTILS_H_
#define UKIVE_GRAPHICS_WIN_IMAGES_IMAGE_OPTIONS_WIN_UTILS_H_

#include <d2d1.h>
#include <wincodec.h>

#include "ukive/graphics/images/image_options.h"


namespace ukive {
namespace win {

    DXGI_FORMAT mapDXGIFormat(ImagePixelFormat f);
    D2D1_ALPHA_MODE mapD2D1AlphaMode(ImageAlphaMode m);
    WICPixelFormatGUID mapWICFormat(const ImageOptions& options);

    D2D1_BITMAP_PROPERTIES mapBitmapProps(const ImageOptions& options);

}
}

#endif  // UKIVE_GRAPHICS_WIN_IMAGES_IMAGE_OPTIONS_WIN_UTILS_H_