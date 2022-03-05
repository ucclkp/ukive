// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_POINT_HPP_
#define UKIVE_GRAPHICS_POINT_HPP_

#include "utils/math/algebra/point.hpp"


namespace ukive {

    template <typename Ty>
    using PointT = utl::math::PointT<Ty, 2>;

    using Point = utl::pt2i;
    using PointF = utl::pt2f;
    using PointD = utl::pt2d;

}

#endif  // UKIVE_GRAPHICS_POINT_HPP_