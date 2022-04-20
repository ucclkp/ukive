// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/image_frame.h"

#include "ukive/app/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/window/window.h"


namespace ukive {

    // static
    GPtr<ImageFrame> ImageFrame::create(Canvas* canvas, const GPtr<LcImageFrame>& frame) {
        return canvas->createImage(frame);
    }

    // static
    GPtr<ImageFrame> ImageFrame::create(Canvas* canvas, int width, int height) {
        return canvas->createImage(width, height);
    }

    // static
    GPtr<ImageFrame> ImageFrame::create(
        Canvas* canvas, int width, int height, const ImageOptions& options)
    {
        return canvas->createImage(width, height, options);
    }

    // static
    GPtr<ImageFrame> ImageFrame::create(
        Canvas* canvas, int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride)
    {
        return canvas->createImage(width, height, pixel_data, stride);
    }

    // static
    GPtr<ImageFrame> ImageFrame::create(
        Canvas* canvas, int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride,
        const ImageOptions& options)
    {
        return canvas->createImage(width, height, pixel_data, stride, options);
    }

    // static
    GPtr<ImageFrame> ImageFrame::decodeFile(
        Canvas* canvas, const std::u16string_view& file_name)
    {
        auto ic = Application::getImageLocFactory();
        auto img = ic->decodeFile(
            file_name, canvas->getBuffer()->getImageOptions());
        if (!img.isValid()) {
            return {};
        }

        return canvas->createImage(img.getFrames()[0]);
    }

    // static
    GPtr<ImageFrame> ImageFrame::decodeFile(
        Canvas* canvas, const std::u16string_view& file_name, const ImageOptions& options)
    {
        auto ic = Application::getImageLocFactory();
        auto img = ic->decodeFile(file_name, options);
        if (!img.isValid()) {
            return {};
        }

        return canvas->createImage(img.getFrames()[0]);
    }

    // static
    GPtr<ImageFrame> ImageFrame::decodeThumbnail(
        Canvas* canvas, const std::u16string_view& file_name, int width, int height)
    {
        auto ic = Application::getImageLocFactory();

        std::string data;
        ImageOptions options;
        auto frame = ic->createThumbnail(file_name, width, height, &options);
        if (!frame) {
            return {};
        }

        return canvas->createImage(frame);
    }

    ImageFrame::ImageFrame(const ImageOptions& options)
        : options_(options) {}

    void ImageFrame::setData(const std::shared_ptr<ImageData>& data) {
        data_ = data;
    }

    const std::shared_ptr<ImageData>& ImageFrame::getData() const {
        return data_;
    }

}
