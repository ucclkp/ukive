// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_LC_IMAGE_FRAME_H_
#define UKIVE_GRAPHICS_IMAGES_LC_IMAGE_FRAME_H_

#include <memory>
#include <string>

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/size.hpp"



namespace ukive {

    class ImageData;

    class LcImageFrame : public virtual GRefCount {
    public:
        static GPtr<LcImageFrame> create(
            int width, int height, const ImageOptions& options);
        static GPtr<LcImageFrame> create(
            int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride,
            const ImageOptions& options);
        static bool saveToFile(
            int width, int height,
            const void* data, size_t len, size_t stride,
            ImageContainer container,
            const ImageOptions& options,
            const std::u16string_view& file_name);

        explicit LcImageFrame(const ImageOptions& options);
        virtual ~LcImageFrame();

        void setData(const std::shared_ptr<ImageData>& data);
        const std::shared_ptr<ImageData>& getData() const;

        const ImageOptions& getOptions() const { return options_; }

        virtual void setDpi(float dpi_x, float dpi_y) = 0;
        virtual void getDpi(float* dpi_x, float* dpi_y) const = 0;

        virtual SizeF getSize() const = 0;
        virtual SizeU getPixelSize() const = 0;

        virtual bool copyPixels(
            size_t stride, void* pixels, size_t buf_size) = 0;
        virtual void* lockPixels(unsigned int flags, size_t* stride) = 0;
        virtual void unlockPixels() = 0;

    private:
        ImageOptions options_;
        std::shared_ptr<ImageData> data_;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_LC_IMAGE_FRAME_H_
