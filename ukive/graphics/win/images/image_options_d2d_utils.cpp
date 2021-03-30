// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_options_d2d_utils.h"

#include "ukive/graphics/images/image_options.h"


namespace ukive {

    D2D1_BITMAP_PROPERTIES mapBitmapProps(const ImageOptions& options) {
        DXGI_FORMAT format;
        switch (options.pixel_format) {
        case ImagePixelFormat::HDR:
            format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case ImagePixelFormat::RAW:
            format = DXGI_FORMAT_UNKNOWN;
            break;
        case ImagePixelFormat::B8G8R8A8_UNORM:
        default:
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        }

        D2D1_ALPHA_MODE alpha_mode;
        switch (options.alpha_mode) {
        case ImageAlphaMode::STRAIGHT:
            alpha_mode = D2D1_ALPHA_MODE_STRAIGHT;
            break;
        case ImageAlphaMode::IGNORED:
            alpha_mode = D2D1_ALPHA_MODE_IGNORE;
            break;

        case ImageAlphaMode::PREMULTIPLIED:
        default:
            alpha_mode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            break;
        }

        auto prop = D2D1::BitmapProperties(D2D1::PixelFormat(format, alpha_mode));

        switch (options.dpi_type) {
        case ImageDPIType::SPECIFIED:
            prop.dpiX = options.dpi_x;
            prop.dpiY = options.dpi_y;
            break;

        case ImageDPIType::DEFAULT:
        default:
            break;
        }

        return prop;
    }

}
