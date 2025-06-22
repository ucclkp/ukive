// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_IMAGE_OPTIONS_H_
#define UKIVE_GRAPHICS_IMAGES_IMAGE_OPTIONS_H_


namespace ukive {

    enum ImageAccessFlags {
        IAF_READ = 1 << 0,
        IAF_WRITE = 1 << 1,
    };

    enum class ImageContainer {
        PNG,
        JPEG,
        BMP,
    };

    enum class ImagePixelFormat {
        R8_UNORM,
        I8_UNORM,      // 仅限图片
        R8G8B8_UNORM,  // 仅限图片
        R8G8B8A8_UNORM,
        B8G8R8_UNORM,  // 仅限图片
        B8G8R8A8_UNORM,
        R16G16B16A16_FLOAT,
        R16G16B16A16_UNORM,
        R10G10B10A2_UNORM,
        RAW,
    };

    enum class ImageAlphaMode {
        PREMULTIPLIED,
        STRAIGHT,
        IGNORED,
    };

    enum class ImageDPIType {
        DEFAULT,
        SPECIFIED,
    };

    class ImageOptions {
    public:
        explicit ImageOptions(
            ImagePixelFormat pf = ImagePixelFormat::B8G8R8A8_UNORM,
            ImageAlphaMode am = ImageAlphaMode::PREMULTIPLIED,
            bool hdr_enabled = false);
        ImageOptions(
            float dpi_x, float dpi_y,
            ImagePixelFormat pf = ImagePixelFormat::B8G8R8A8_UNORM,
            ImageAlphaMode am = ImageAlphaMode::PREMULTIPLIED,
            bool hdr_enabled = false);

        bool operator==(const ImageOptions& rhs) const;
        bool operator!=(const ImageOptions& rhs) const;

        float dpi_x;
        float dpi_y;
        ImagePixelFormat pixel_format;
        ImageAlphaMode alpha_mode;
        ImageDPIType dpi_type;
        bool hdr_enabled;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_IMAGE_OPTIONS_H_