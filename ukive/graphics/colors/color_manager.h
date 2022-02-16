// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_COLOR_MANAGER_H_
#define UKIVE_GRAPHICS_COLORS_COLOR_MANAGER_H_

#include "ukive/graphics/colors/color.h"


namespace ukive {

    class ColorManager {
    public:
        static ColorManager* create();

        virtual ~ColorManager() = default;

        virtual bool convertColor(const Color& src, Color* dst) = 0;

        static bool getDefaultProfile(std::u16string* path);
    };

}

#endif  // UKIVE_GRAPHICS_COLORS_COLOR_MANAGER_H_