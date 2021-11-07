// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_THEME_INFO_H_
#define UKIVE_SYSTEM_THEME_INFO_H_

#include "ukive/graphics/colors/color.h"


namespace ukive {

    struct ThemeConfig {
        enum Type : uint32_t {
            COLOR_CHANGED   = 1 << 0,
            COLOR_EXISTANCE = 1 << 1,
            CONFIG_CHANGED  = 1 << 2,
        };

        uint32_t type = 0;
        bool has_color;
        Color primary_color;

        bool light_theme;
        bool transparency_enabled;
    };

    enum TranslucentType {
        TRANS_OPAQUE = 1 << 0,
        TRANS_SYSTEM = 1 << 1,
        TRANS_BLURBEHIND = 1 << 2,
        TRANS_TRANSPARENT = 1 << 3,
        TRANS_LAYERED = 1 << 4,
        TRANS_BLURBEHIND_SYSTEM = TRANS_BLURBEHIND | TRANS_SYSTEM,
        TRANS_TRANSPARENT_SYSTEM = TRANS_TRANSPARENT | TRANS_SYSTEM,
    };

}

#endif  // UKIVE_SYSTEM_THEME_INFO_H_