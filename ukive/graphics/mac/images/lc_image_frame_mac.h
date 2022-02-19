// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_IMAGES_LC_IMAGE_FRAME_MAC_H_
#define UKIVE_GRAPHICS_MAC_IMAGES_LC_IMAGE_FRAME_MAC_H_

#include "ukive/graphics/images/lc_image_frame.h"

#include <CoreGraphics/CGImage.h>


namespace ukive {
namespace mac {

    class LcImageFrameMac : public LcImageFrame {
    public:
        explicit LcImageFrameMac(CGImageRef img);
        ~LcImageFrameMac();

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float* dpi_x, float* dpi_y) const override;

        SizeF getSize() const override;
        SizeU getPixelSize() const override;

        bool copyPixels(size_t stride, void* pixels, size_t buf_size) override;
        void* lockPixels() override;
        void unlockPixels() override;

        CGImageRef getNative() const;

    private:
        float dpi_x_;
        float dpi_y_;
        CGImageRef img_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_IMAGES_LC_IMAGE_FRAME_MAC_H_
