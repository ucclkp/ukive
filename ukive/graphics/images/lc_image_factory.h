// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGE_LC_IMAGE_FACTORY_H_
#define UKIVE_GRAPHICS_IMAGE_LC_IMAGE_FACTORY_H_

#include <string>

#include "ukive/graphics/images/image_options.h"


namespace ukive {

    class LcImage;
    class LcImageFrame;

    class LcImageFactory {
    public:
        static LcImageFactory* create();

        virtual ~LcImageFactory() = default;

        virtual bool initialization() = 0;
        virtual void destroy() = 0;

        virtual LcImageFrame* create(
            int width, int height, const ImageOptions& options) = 0;
        virtual LcImageFrame* create(
            int width, int height,
            uint8_t* pixel_data, size_t size, size_t stride,
            const ImageOptions& options) = 0;

        virtual LcImage decodeFile(
            const std::u16string& file_name, const ImageOptions& options) = 0;
        virtual LcImage decodeMemory(
            uint8_t* buffer, size_t size, const ImageOptions& options) = 0;

        virtual bool getThumbnail(
            const std::u16string& file_name,
            int frame_width, int frame_height,
            std::string* out, int* real_w, int* real_h, ImageOptions* options) = 0;

        virtual bool saveToPNGFile(
            int width, int height,
            uint8_t* data, size_t byte_count, size_t stride,
            const ImageOptions& options,
            const std::u16string& file_name) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGE_LC_IMAGE_FACTORY_H_
