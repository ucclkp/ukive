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
            std::abs(f1 - f2) < (std::numeric_limits<float>::min)())
        {
            return true;
        }
        return false;
    }

}

namespace ukive {

    ImageOptions::ImageOptions(ImagePixelFormat pf, ImageAlphaMode am, bool hdr_enabled)
        : ImageOptions(0, 0, pf, am, hdr_enabled) {}

    ImageOptions::ImageOptions(
        float dpi_x, float dpi_y,
        ImagePixelFormat pf, ImageAlphaMode am, bool hdr_enabled)
        : dpi_x(dpi_x), dpi_y(dpi_y),
          pixel_format(pf), alpha_mode(am),
          dpi_type(ImageDPIType::SPECIFIED),
          hdr_enabled(hdr_enabled),
          sdr_white_level(80.f) {}

    bool ImageOptions::operator==(const ImageOptions& rhs) const {
        if (pixel_format == rhs.pixel_format &&
            alpha_mode == rhs.alpha_mode &&
            dpi_type == rhs.dpi_type &&
            hdr_enabled == rhs.hdr_enabled)
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
