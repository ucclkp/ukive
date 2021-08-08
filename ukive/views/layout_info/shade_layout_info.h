// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_SHADE_LAYOUT_INFO_H_
#define UKIVE_VIEWS_LAYOUT_INFO_SHADE_LAYOUT_INFO_H_

#include "ukive/graphics/rect.hpp"
#include "ukive/views/layout_info/gravity.h"
#include "ukive/views/layout_info/layout_info.h"


namespace ukive {

    class ShadeLayoutInfo : public LayoutInfo {
    public:
        ShadeLayoutInfo();

        Rect anchor;
        int gravity = GV_NONE;
        bool is_max_visible = true;
        bool is_discretized = true;

        int adj_gravity = GV_NONE;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_SHADE_LAYOUT_INFO_H_