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
    LcImageFrame* LcImageFrame::create(int width, int height, const ImageOptions& options) {
        auto ic = Application::getImageLocFactory();
        return ic->create(width, height, options);
    }

    // static
    bool LcImageFrame::saveToPngFile(
        int width, int height,
        uint8_t* data, size_t byte_count, size_t stride,
        const ImageOptions& options,
        const std::u16string& file_name)
    {
        auto ic = Application::getImageLocFactory();
        return ic->saveToPNGFile(width, height, data, byte_count, stride, options, file_name);
    }

    LcImageFrame::LcImageFrame() {
    }

    LcImageFrame::~LcImageFrame() {
    }

    void LcImageFrame::setData(const std::shared_ptr<ImageData>& data) {
        data_ = data;
    }

    const std::shared_ptr<ImageData>& LcImageFrame::getData() const {
        return data_;
    }

}
