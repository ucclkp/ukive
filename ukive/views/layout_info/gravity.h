// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_GRAVITY_H_
#define UKIVE_VIEWS_LAYOUT_INFO_GRAVITY_H_

#include "ukive/graphics/rect.hpp"
#include "ukive/graphics/size.hpp"


namespace ukive {

    enum Gravity {
        GV_NONE          = 0,

        GV_START         = 1 << 0,
        GV_MID_START     = 1 << 1,
        GV_MID_HORI      = 1 << 2,
        GV_MID_END       = 1 << 3,
        GV_END           = 1 << 4,

        GV_TOP           = 1 << 5,
        GV_MID_TOP       = 1 << 6,
        GV_MID_VERT      = 1 << 7,
        GV_MID_BOTTOM    = 1 << 8,
        GV_BOTTOM        = 1 << 9,
    };

    void calculateGravityBounds(
        const Rect& root, bool is_max_visible, bool is_discretized,
        const Rect& anchor, int gravity, const Size& target_size, Rect* out, int* adj_gravity);

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_GRAVITY_H_