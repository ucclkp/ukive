// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_VECTOR2_HPP_
#define UKIVE_GRAPHICS_VECTOR2_HPP_

#include <cmath>


namespace ukive {

    template <typename Ty>
    class Vector2T {
    public:
        using type = Ty;

        Vector2T()
            : x(0), y(0) {}
        Vector2T(Ty x, Ty y)
            : x(x), y(y) {}

        bool operator==(const Vector2T& rhs) const {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const Vector2T& rhs) const {
            return x != rhs.x || y != rhs.y;
        }

        Vector2T operator+(const Vector2T& rhs) const {
            return Vector2T(x + rhs.x, y + rhs.y);
        }
        Vector2T operator-(const Vector2T& rhs) const {
            return Vector2T(x - rhs.x, y - rhs.y);
        }
        Vector2T operator*(Ty value) const {
            return Vector2T(x * value, y * value);
        }
        Vector2T operator*(const Vector2T& rhs) const {
            return Vector2T(x * rhs.x, y * rhs.y);
        }
        Vector2T operator/(Ty value) const {
            return Vector2T(x / value, y / value);
        }

        void normalize() {
            Ty l = length();
            x /= l;
            y /= l;
        }

        Ty length() const {
            return std::sqrt(x * x + y * y);
        }

        Ty x, y;
    };

    using Vector2 = Vector2T<float>;
}

#endif  // UKIVE_GRAPHICS_VECTOR2_HPP_