// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_GEOCAL_HPP_
#define UKIVE_GRAPHICS_3D_GEOCAL_HPP_

#include "ukive/graphics/3d/matrix.hpp"


namespace ukv3d {

    /**
     * 计算点 p 到直线 p1-p2 的距离。
     */
    template <typename Ty>
    Ty distanceLinePoint(
        const Point3T<Ty>& p1, const Point3T<Ty>& p2, const Point3T<Ty>& p)
    {
        auto pv = p - p1;
        auto lv = p2 - p1;
        auto length_sq = lv.lengthSq();

        auto scale = pv * lv / length_sq;
        auto distance = pv - lv * scale;

        return distance.length();
    }

}

#endif  // UKIVE_GRAPHICS_3D_GEOCAL_HPP_