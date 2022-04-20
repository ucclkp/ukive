// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_IMAGE_FRAME_H_
#define UKIVE_GRAPHICS_IMAGES_IMAGE_FRAME_H_

#include <memory>
#include <string>

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/size.hpp"


namespace ukive {

    class Canvas;
    class Window;
    class LcImageFrame;
    class ImageData;

    class ImageFrame : public virtual GRefCount {
    public:
        static GPtr<ImageFrame> create(Canvas* canvas, const GPtr<LcImageFrame>& frame);
        static GPtr<ImageFrame> create(Canvas* canvas, int width, int height);
        static GPtr<ImageFrame> create(
            Canvas* canvas, int width, int height, const ImageOptions& options);
        static GPtr<ImageFrame> create(
            Canvas* canvas, int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride);
        static GPtr<ImageFrame> create(
            Canvas* canvas, int width, int height,
            const GPtr<ByteData>& pixel_data, size_t stride,
            const ImageOptions& options);

        static GPtr<ImageFrame> decodeFile(
            Canvas* canvas, const std::u16string_view& file_name);
        static GPtr<ImageFrame> decodeFile(
            Canvas* canvas, const std::u16string_view& file_name, const ImageOptions& options);
        static GPtr<ImageFrame> decodeThumbnail(
            Canvas* canvas, const std::u16string_view& file_name, int width, int height);

        explicit ImageFrame(const ImageOptions& options);
        virtual ~ImageFrame() = default;

        void setData(const std::shared_ptr<ImageData>& data);
        const std::shared_ptr<ImageData>& getData() const;

        const ImageOptions& getOptions() const { return options_; }

        virtual void setDpi(float dpi_x, float dpi_y) = 0;
        virtual void getDpi(float* dpi_x, float* dpi_y) const = 0;

        virtual SizeF getSize() const = 0;
        virtual SizeU getPixelSize() const = 0;

    private:
        ImageOptions options_;
        std::shared_ptr<ImageData> data_;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_IMAGE_FRAME_H_
