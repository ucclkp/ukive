// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGE_LC_IMAGE_FACTORY_H_
#define UKIVE_GRAPHICS_IMAGE_LC_IMAGE_FACTORY_H_

#include <string>

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/images/image_options.h"


namespace ukive {

    class LcImage;
    class LcImageFrame;

    class LcImageFactory {
    public:
        static LcImageFactory* create();

        virtual ~LcImageFactory() = default;

        virtual bool initialize() = 0;
        virtual void destroy() = 0;

        virtual GPtr<LcImageFrame> create(
            int width, int height, const ImageOptions& options) = 0;
        virtual GPtr<LcImageFrame> create(
            int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride,
            const ImageOptions& options) = 0;
        virtual GPtr<LcImageFrame> createThumbnail(
            const std::u16string_view& file_name,
            int frame_width, int frame_height, ImageOptions* options) = 0;

        virtual LcImage decodeFile(
            const std::u16string_view& file_name, const ImageOptions& options) = 0;
        virtual LcImage decodeMemory(
            const void* buffer, size_t size, const ImageOptions& options) = 0;

        virtual bool saveToFile(
            int width, int height,
            const void* data, size_t len, size_t stride,
            ImageContainer container,
            const ImageOptions& options,
            const std::u16string_view& file_name) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGE_LC_IMAGE_FACTORY_H_
