// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/images/lc_image_frame_mac.h"

#include "utils/number.hpp"


namespace ukive {

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

    Size LcImageFrameMac::getSize() const {
        if (img_) {
            auto width = CGImageGetWidth(img_);
            auto height = CGImageGetHeight(img_);
            return Size(utl::num_cast<int>(width), utl::num_cast<int>(height));
        }
        return Size(0, 0);
    }

    bool LcImageFrameMac::getPixels(std::string *out, int *width, int *height) {
        return false;
    }

    bool LcImageFrameMac::getBWPixels(std::string *out, int *width, int *height) {
        return false;
    }

    CGImageRef LcImageFrameMac::getNative() const {
        return img_;
    }

}
