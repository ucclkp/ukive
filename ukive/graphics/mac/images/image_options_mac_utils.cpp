// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_options_mac_utils.h"

#include "ukive/graphics/images/image_options.h"

#import <CoreGraphics/CoreGraphics.h>


namespace ukive {
namespace mac {

    uint32_t mapCGBitmapContextInfo(const ImageOptions& options) {
        uint32_t info = kCGImageByteOrder32Little;
        switch (options.pixel_format) {
            case ImagePixelFormat::HDR:
                info |= kCGBitmapFloatComponents;
                break;
            case ImagePixelFormat::RAW:
            case ImagePixelFormat::B8G8R8A8_UNORM:
            default:
                break;
        }

        switch (options.alpha_mode) {
            case ImageAlphaMode::IGNORED:
                info |= kCGImageAlphaNoneSkipFirst;
                break;
            case ImageAlphaMode::STRAIGHT:
                info |= kCGImageAlphaFirst;
                break;
            case ImageAlphaMode::PREMULTIPLIED:
            default:
                info |= kCGImageAlphaPremultipliedFirst;
                break;
        }

        return info;
    }

}
}
