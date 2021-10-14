// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_LC_IMAGE_FRAME_H_
#define UKIVE_GRAPHICS_IMAGES_LC_IMAGE_FRAME_H_

#include <string>

#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/size.hpp"



namespace ukive {

    class ImageData;
    class ImageOptions;

    class LcImageFrame {
    public:
        static LcImageFrame* create(int width, int height, const ImageOptions& options);
        static bool saveToFile(
            int width, int height,
            uint8_t* data, size_t byte_count, size_t stride,
            ImageContainer container,
            const ImageOptions& options,
            const std::u16string& file_name);

        LcImageFrame();
        virtual ~LcImageFrame();

        void setData(const std::shared_ptr<ImageData>& data);
        const std::shared_ptr<ImageData>& getData() const;

        virtual void setDpi(float dpi_x, float dpi_y) = 0;
        virtual void getDpi(float* dpi_x, float* dpi_y) const = 0;

        virtual SizeF getSize() const = 0;
        virtual SizeU getPixelSize() const = 0;

        virtual bool copyPixels(
            size_t stride, uint8_t* pixels, size_t buf_size) = 0;
        virtual uint8_t* lockPixels() = 0;
        virtual void unlockPixels() = 0;

    private:
        std::shared_ptr<ImageData> data_;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_LC_IMAGE_FRAME_H_
