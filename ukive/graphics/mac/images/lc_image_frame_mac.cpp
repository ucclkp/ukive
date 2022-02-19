// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/images/lc_image_frame_mac.h"

#include "utils/numbers.hpp"

#include "ukive/window/window_dpi_utils.h"


namespace ukive {
namespace mac {

    LcImageFrameMac::LcImageFrameMac(CGImageRef img)
        : img_(img),
          dpi_x_(72.f),
          dpi_y_(72.f) {}

    LcImageFrameMac::~LcImageFrameMac() {
        CGImageRelease(img_);
    }

    void LcImageFrameMac::setDpi(float dpi_x, float dpi_y) {
        if (dpi_x > 0 && dpi_y > 0) {
            dpi_x_ = dpi_x;
            dpi_y_ = dpi_y;
        }
    }

    void LcImageFrameMac::getDpi(float* dpi_x, float* dpi_y) const {
        *dpi_x = dpi_x_;
        *dpi_y = dpi_y_;
    }

    SizeF LcImageFrameMac::getSize() const {
        if (img_) {
            float dpi_x, dpi_y;
            getDpi(&dpi_x, &dpi_y);

            auto width = CGImageGetWidth(img_);
            auto height = CGImageGetHeight(img_);
            return SizeF(
                width / (dpi_x / kDefaultDpi),
                height / (dpi_y / kDefaultDpi));
        }
        return SizeF(0, 0);
    }

    SizeU LcImageFrameMac::getPixelSize() const {
        if (img_) {
            auto width = CGImageGetWidth(img_);
            auto height = CGImageGetHeight(img_);
            return SizeU(
                utl::num_cast<unsigned int>(width),
                utl::num_cast<unsigned int>(height));
        }
        return SizeU(0, 0);
    }

    bool LcImageFrameMac::copyPixels(size_t stride, void* pixels, size_t buf_size) {
        return false;
    }

    void* LcImageFrameMac::lockPixels() {
        return nullptr;
    }

    void LcImageFrameMac::unlockPixels() {
    }

    CGImageRef LcImageFrameMac::getNative() const {
        return img_;
    }

}
}
