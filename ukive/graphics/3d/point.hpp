// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_POINT_H_
#define UKIVE_GRAPHICS_3D_POINT_H_

#include "vector.hpp"


namespace ukv3d {

    template<typename Ty>
    class Point2T {
    public:
        Point2T() : x(0), y(0) {}
        Point2T(Ty _x, Ty _y) : x(_x), y(_y) {}

        Point2T operator+(const Vector2T<Ty>& rhs) const {
            return Point2T(*this).add(rhs);
        }
        Vector2T<Ty> operator-(const Point2T& rhs) const {
            return Vector2T<Ty>(x - rhs.x, y - rhs.y);
        }
        Point2T operator-(const Vector2T<Ty>& rhs) const {
            return Point2T(*this).sub(rhs);
        }
        bool operator==(const Point2T& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const Point2T& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator Point2T<C>() const {
            return Point2T<C>(static_cast<C>(x), static_cast<C>(y));
        }

        Point2T& add(const Vector2T<Ty>& rhs) {
            x += rhs.x; y += rhs.y;
            return *this;
        }

        Point2T& sub(const Vector2T<Ty>& rhs) {
            x -= rhs.x; y -= rhs.y;
            return *this;
        }

        bool equal(const Point2T& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        void set(Ty _x, Ty _y) {
            x = _x; y = _y;
        }

        Ty x, y;
    };

    template<typename Ty>
    class Point3T {
    public:
        Point3T() : x(0), y(0), z(0) {}
        Point3T(Ty x, Ty y, Ty z) : x(x), y(y), z(z) {}
        Point3T(const Point2T<Ty>& p, Ty z) : x(p.x), y(p.y), z(z) {}

        Point3T operator+(const Vector3T<Ty>& rhs) const {
            return Point3T(*this).add(rhs);
        }
        Vector3T<Ty> operator-(const Point3T& rhs) const {
            return Vector3T<Ty>(x - rhs.x, y - rhs.y, z - rhs.z);
        }
        Point3T operator-(const Vector3T<Ty>& rhs) const {
            return Point3T(*this).sub(rhs);
        }
        Vector3T<Ty> toVector() const {
            return Vector3T<Ty>(x, y, z);
        }
        bool operator==(const Point3T& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const Point3T& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator Point3T<C>() const {
            return Point3T<C>(
                static_cast<C>(x),
                static_cast<C>(y),
                static_cast<C>(z));
        }

        Point3T& add(const Vector3T<Ty>& rhs) {
            x += rhs.x; y += rhs.y; z += rhs.z;
            return *this;
        }

        Point3T& sub(const Vector3T<Ty>& rhs) {
            x -= rhs.x; y -= rhs.y; z -= rhs.z;
            return *this;
        }

        bool equal(const Point3T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        void set(Ty _x, Ty _y, Ty _z) {
            x = _x; y = _y; z = _z;
        }

        Ty x, y, z;
    };

    template<typename Ty>
    class Point4T {
    public:
        Point4T() : x(0), y(0), z(0), w(0) {}
        Point4T(Ty x, Ty y, Ty z, Ty w) : x(x), y(y), z(z), w(w) {}
        Point4T(const Point3T<Ty>& p, Ty w) : x(p.x), y(p.y), z(p.z), w(w) {}

        Point4T operator+(const Vector4T<Ty>& rhs) const {
            return Point4T(*this).add(rhs);
        }
        Vector4T<Ty> operator-(const Point4T& rhs) const {
            return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }
        Point4T operator-(const Vector4T<Ty>& rhs) const {
            return Point4T(*this).sub(rhs);
        }
        Vector4T<Ty> toVector() const {
            return Vector4T<Ty>(x, y, z, w);
        }
        bool operator==(const Point4T& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const Point4T& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator Point4T<C>() const {
            return Point4T<C>(x, y, z, w);
        }

        Point4T& add(const Vector4T<Ty>& rhs) {
            x += rhs.x; y += rhs.y;
            z += rhs.z; w += rhs.w;
            return *this;
        }

        Point4T& sub(const Vector4T<Ty>& rhs) {
            x -= rhs.x; y -= rhs.y;
            z -= rhs.z; w -= rhs.w;
            return *this;
        }

        bool equal(const Point4T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        void set(Ty _x, Ty _y, Ty _z, Ty _w) {
            x = _x; y = _y; z = _z; w = _w;
        }

        Point3T<Ty> p3() const { return Point3T<Ty>(x, y, z); }

        Ty x, y, z, w;
    };


    using Point2F = Point2T<float>;
    using Point3F = Point3T<float>;
    using Point4F = Point4T<float>;
    using Point2D = Point2T<double>;
    using Point3D = Point3T<double>;
    using Point4D = Point4T<double>;

}

#endif  // UKIVE_GRAPHICS_3D_POINT_H_