// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/lc_image_frame.h"

#include "ukive/app/application.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/graphics/images/image_data.h"


namespace ukive {

    // static
    GPtr<LcImageFrame> LcImageFrame::create(
        int width, int height, const ImageOptions& options)
    {
        auto ic = Application::getImageLocFactory();
        return ic->create(width, height, options);
    }

    // static
    GPtr<LcImageFrame> LcImageFrame::create(
        int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride,
        const ImageOptions& options)
    {
        auto ic = Application::getImageLocFactory();
        return ic->create(width, height, pixel_data, stride, options);
    }

    // static
    bool LcImageFrame::saveToFile(
        int width, int height,
        const void* data, size_t len, size_t stride,
        ImageContainer container,
        const ImageOptions& options,
        const std::u16string_view& file_name)
    {
        auto ic = Application::getImageLocFactory();
        return ic->saveToFile(
            width, height, data, len, stride,
            container, options, file_name);
    }

    LcImageFrame::LcImageFrame(const ImageOptions& options)
        : options_(options) {}

    LcImageFrame::~LcImageFrame() {
    }

    void LcImageFrame::setData(const std::shared_ptr<ImageData>& data) {
        data_ = data;
    }

    const std::shared_ptr<ImageData>& LcImageFrame::getData() const {
        return data_;
    }

}
