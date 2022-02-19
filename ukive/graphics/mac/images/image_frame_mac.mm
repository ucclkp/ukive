// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/images/image_frame_mac.h"

#include "utils/numbers.hpp"

#import <Cocoa/Cocoa.h>


namespace ukive {
namespace mac {

    ImageFrameMac::ImageFrameMac(NSBitmapImageRep* img, bool already_flipped, uint8_t* buf)
        : img_(img),
          buf_(buf),
          already_flipped_(already_flipped) {}

    ImageFrameMac::~ImageFrameMac() {
        [img_ release];
        delete buf_;
    }

    void ImageFrameMac::setDpi(float dpi_x, float dpi_y) {
        if (dpi_x > 0 && dpi_y > 0) {
            [img_ setSize:NSMakeSize(
                                     img_.pixelsWide / (dpi_x / 72.f),
                                     img_.pixelsHigh / (dpi_y / 72.f))];
        } else {
            [img_ setSize:NSMakeSize(img_.pixelsWide, img_.pixelsHigh)];
        }
    }

    void ImageFrameMac::getDpi(float *dpi_x, float *dpi_y) const {
        *dpi_x = img_.pixelsWide / float(img_.size.width) * 72.f;
        *dpi_y = img_.pixelsHigh / float(img_.size.height) * 72.f;
    }

    SizeF ImageFrameMac::getSize() const {
        SizeF result;
        if (img_) {
            auto size = img_.size;
            result.set(size.width, size.height);
        } else {
            result.set(0, 0);
        }
        return result;
    }

    SizeU ImageFrameMac::getPixelSize() const {
        return SizeU(
            utl::num_cast<unsigned int>(img_.pixelsWide),
            utl::num_cast<unsigned int>(img_.pixelsHigh));
    }

    bool ImageFrameMac::alreadyFilpped() const {
        return already_flipped_;
    }

    NSBitmapImageRep* ImageFrameMac::getNative() const {
        return img_;
    }

}
}
