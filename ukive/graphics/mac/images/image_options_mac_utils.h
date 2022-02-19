// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_IMAGES_IMAGE_OPTIONS_MAC_UTILS_H_
#define UKIVE_GRAPHICS_MAC_IMAGES_IMAGE_OPTIONS_MAC_UTILS_H_

#include <cstdint>


namespace ukive {

    class ImageOptions;

namespace mac {

    uint32_t mapCGBitmapContextInfo(const ImageOptions& options);

}
}

#endif  // UKIVE_GRAPHICS_MAC_IMAGES_IMAGE_OPTIONS_MAC_UTILS_H_
