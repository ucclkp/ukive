// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_IMAGE_DATA_H_
#define UKIVE_GRAPHICS_IMAGES_IMAGE_DATA_H_

#include "ukive/graphics/colors/color.h"
#include "ukive/graphics/images/jpeg_appmarker_parser.h"


namespace ukive {

    class ImageData {
    public:
        enum Type {
            GIF_IMAGE_DATA = 1,
            GIF_FRAME_DATA = 2,
            JPEG_IMAGE_DATA = 3,
        };

        virtual ~ImageData() = default;

        virtual int getType() const = 0;
    };


    class GifImageFrData : public ImageData {
    public:
        enum DisposalMethods {
            UNSPECIFIED = 0,
            NONE,
            BACKGROUND,
            PREVIOUS
        };

        GifImageFrData() = default;

        int getType() const override { return GIF_FRAME_DATA; }

        int left = 0;
        int top = 0;
        int width = 0;
        int height = 0;
        int disposal = 0;
        bool interlace = false;
        bool sort = false;
        int frame_interval = 0;
    };

    class GifImageData : public ImageData {
    public:
        GifImageData() = default;

        int getType() const override { return GIF_IMAGE_DATA; }

        int width = 0;
        int height = 0;
        int loop_count = 0;
        Color bg_color;
    };

    class JpegImageData : public ImageData {
    public:
        JpegImageData() = default;

        int getType() const override { return JPEG_IMAGE_DATA; }

        jpeg_metadata metadata;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_IMAGE_DATA_H_