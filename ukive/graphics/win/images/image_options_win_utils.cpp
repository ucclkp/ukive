// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_options_win_utils.h"

#include <cassert>


namespace ukive {
namespace win {

    DXGI_FORMAT mapDXGIFormat(ImagePixelFormat f) {
        DXGI_FORMAT format;
        switch (f) {
        case ImagePixelFormat::HDR:
            format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case ImagePixelFormat::RAW:
            format = DXGI_FORMAT_UNKNOWN;
            break;
        case ImagePixelFormat::R8_UNORM:
            format = DXGI_FORMAT_R8_UNORM;
            break;
        case ImagePixelFormat::R8G8B8A8_UNORM:
            format = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case ImagePixelFormat::B8G8R8A8_UNORM:
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        default:
            assert("Failed to map DXGI Format!" && false);
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        }
        return format;
    }

    D2D1_ALPHA_MODE mapD2D1AlphaMode(ImageAlphaMode m) {
        D2D1_ALPHA_MODE alpha_mode;
        switch (m) {
        case ImageAlphaMode::STRAIGHT:
            alpha_mode = D2D1_ALPHA_MODE_STRAIGHT;
            break;
        case ImageAlphaMode::IGNORED:
            alpha_mode = D2D1_ALPHA_MODE_IGNORE;
            break;

        case ImageAlphaMode::PREMULTIPLIED:
            alpha_mode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            break;
        default:
            assert("Failed to map D2D1 alpha mode!" && false);
            alpha_mode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            break;
        }
        return alpha_mode;
    }

    WICPixelFormatGUID mapWICFormat(const ImageOptions& options) {
        WICPixelFormatGUID format;
        switch (options.pixel_format) {
        case ImagePixelFormat::HDR:
            switch (options.alpha_mode) {
            case ImageAlphaMode::STRAIGHT:
                format = GUID_WICPixelFormat64bppRGBAHalf;
                break;
            case ImageAlphaMode::IGNORED:
                format = GUID_WICPixelFormat64bppRGBHalf;
                break;
            case ImageAlphaMode::PREMULTIPLIED:
            default:
                format = GUID_WICPixelFormat64bppPRGBAHalf;
                break;
            }
            break;

        case ImagePixelFormat::R8_UNORM:
            format = GUID_WICPixelFormat8bppGray;
            break;
        case ImagePixelFormat::I8_UNORM:
            format = GUID_WICPixelFormat8bppIndexed;
            break;
        case ImagePixelFormat::R8G8B8_UNORM:
            format = GUID_WICPixelFormat24bppRGB;
            break;

        case ImagePixelFormat::R8G8B8A8_UNORM:
            switch (options.alpha_mode) {
            case ImageAlphaMode::STRAIGHT:
                format = GUID_WICPixelFormat32bppRGBA;
                break;
            case ImageAlphaMode::IGNORED:
                format = GUID_WICPixelFormat32bppRGB;
                break;
            case ImageAlphaMode::PREMULTIPLIED:
            default:
                format = GUID_WICPixelFormat32bppPRGBA;
                break;
            }
            break;

        case ImagePixelFormat::RAW:
        case ImagePixelFormat::B8G8R8A8_UNORM:
        default:
            switch (options.alpha_mode) {
            case ImageAlphaMode::STRAIGHT:
                format = GUID_WICPixelFormat32bppBGRA;
                break;
            case ImageAlphaMode::IGNORED:
                format = GUID_WICPixelFormat32bppBGR;
                break;
            case ImageAlphaMode::PREMULTIPLIED:
            default:
                format = GUID_WICPixelFormat32bppPBGRA;
                break;
            }
            break;
        }

        return format;
    }

    D2D1_BITMAP_PROPERTIES mapBitmapProps(const ImageOptions& options) {
        auto format = mapDXGIFormat(options.pixel_format);
        auto alpha_mode = mapD2D1AlphaMode(options.alpha_mode);
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
}
