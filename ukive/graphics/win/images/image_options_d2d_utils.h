// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGES_IMAGE_OPTIONS_D2D_UTILS_H_
#define UKIVE_GRAPHICS_WIN_IMAGES_IMAGE_OPTIONS_D2D_UTILS_H_

#include <d2d1.h>


namespace ukive {

    class ImageOptions;

    D2D1_BITMAP_PROPERTIES mapBitmapProps(const ImageOptions& options);

}

#endif  // UKIVE_GRAPHICS_WIN_IMAGES_IMAGE_OPTIONS_D2D_UTILS_H_