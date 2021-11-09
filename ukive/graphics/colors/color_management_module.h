// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_COLOR_MANAGEMENT_MODULE_H_
#define UKIVE_GRAPHICS_COLORS_COLOR_MANAGEMENT_MODULE_H_

#include "ukive/graphics/colors/color.h"


namespace ukive {

    class ColorManagementModule {
    public:
        ColorManagementModule();
        ~ColorManagementModule();

        bool convertColor(
            const std::u16string& icc_path, const Color& src, Color* dst);
    };

}

#endif  // UKIVE_GRAPHICS_COLORS_COLOR_MANAGEMENT_MODULE_H_