// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_options.h"

#include <cmath>
#include <limits>


namespace {

    bool isEqual(float f1, float f2) {
        if (std::abs(f1 - f2) <= std::numeric_limits<float>::epsilon() * std::abs(f1 - f2) ||
            std::abs(f1 - f2) < std::numeric_limits<float>::min())
        {
            return true;
        }
        return false;
    }

}

namespace ukive {

    ImageOptions::ImageOptions(ImagePixelFormat pf, ImageAlphaMode am)
        : pixel_format(pf), alpha_mode(am), dpi_type(ImageDPIType::DEFAULT) {}

    ImageOptions::ImageOptions(
        float dpi_x, float dpi_y,
        ImagePixelFormat pf, ImageAlphaMode am)
        : dpi_x(dpi_x), dpi_y(dpi_y),
          pixel_format(pf), alpha_mode(am), dpi_type(ImageDPIType::SPECIFIED) {}

    bool ImageOptions::operator==(const ImageOptions& rhs) const {
        if (pixel_format == rhs.pixel_format &&
            alpha_mode == rhs.alpha_mode &&
            dpi_type == rhs.dpi_type)
        {
            if (dpi_type == ImageDPIType::SPECIFIED) {
                return isEqual(dpi_x, rhs.dpi_x) && isEqual(dpi_y, rhs.dpi_y);
            }
            return true;
        }
        return false;
    }

    bool ImageOptions::operator!=(const ImageOptions& rhs) const {
        return !operator==(rhs);
    }

}
