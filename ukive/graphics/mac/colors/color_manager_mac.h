// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_COLORS_COLOR_MANAGER_MAC_H_
#define UKIVE_GRAPHICS_MAC_COLORS_COLOR_MANAGER_MAC_H_

#include "ukive/graphics/colors/color.h"

#include "ukive/graphics/colors/color_manager.h"


namespace ukive {
namespace mac {

    class ColorManagerMac : public ColorManager {
    public:
        ColorManagerMac();

        bool convertColor(const Color& src, Color* dst) override;

        static bool getDefaultProfile(std::u16string* path);

    private:
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_COLORS_COLOR_MANAGER_MAC_H_
