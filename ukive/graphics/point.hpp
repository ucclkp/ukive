// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_POINT_H_
#define UKIVE_GRAPHICS_POINT_H_

#include "ukive/graphics/vector2.hpp"


namespace ukive {

    template <typename Ty>
    class PointT {
    public:
        using type = Ty;

        PointT()
            : x(0), y(0) {}
        PointT(Ty x, Ty y)
            : x(x), y(y) {}

        template <typename C>
        explicit operator PointT<C>() const {
            return PointT<C>(static_cast<C>(x), static_cast<C>(y));
        }

        bool operator==(const PointT& rhs) const {
            return (x == rhs.x && y == rhs.y);
        }

        bool operator!=(const PointT& rhs) const {
            return (x != rhs.x || y != rhs.y);
        }

        PointT operator+(const Vector2& vec) const {
            return PointT(x + vec.x, y + vec.y);
        }

        Vector2 operator-(const PointT& rhs) const {
            return Vector2(x - rhs.x, y - rhs.y);
        }

        void set(Ty x, Ty y) {
            this->x = x;
            this->y = y;
        }

        Ty x, y;
    };

    using Point = PointT<int>;
    using PointF = PointT<float>;
    using PointD = PointT<double>;

}

#endif  // UKIVE_GRAPHICS_POINT_H_