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

    class LcImageFrameMac : public LcImageFrame {
    public:
        explicit LcImageFrameMac(CGImageRef img);
        ~LcImageFrameMac();

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float* dpi_x, float* dpi_y) const override;

        Size getSize() const override;
        bool getPixels(std::string *out, int *width, int *height) override;
        bool getBWPixels(std::string *out, int *width, int *height) override;

        CGImageRef getNative() const;

    private:
        float dpi_x_;
        float dpi_y_;
        CGImageRef img_;
    };

}

#endif  // UKIVE_GRAPHICS_MAC_IMAGES_LC_IMAGE_FRAME_MAC_H_
