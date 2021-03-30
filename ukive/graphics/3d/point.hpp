// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_POINT_H_
#define UKIVE_GRAPHICS_3D_POINT_H_

#include "vector.hpp"


namespace ukv3d {

    template<typename T>
    class Point2T {
    public:
        Point2T() : x(0), y(0) {}
        Point2T(T _x, T _y) : x(_x), y(_y) {}

        Point2T operator+(const Vector2T<T>& rhs) const { return Point2T(*this).add(rhs); }
        Vector2T<T> operator-(const Point2T& rhs) const { return Vector2T<T>(x - rhs.x, y - rhs.y); }
        Point2T operator-(const Vector2T<T>& rhs) const { return Point2T(*this).sub(rhs); }
        bool operator==(const Point2T& rhs) const { return isEqual(rhs); }
        bool operator!=(const Point2T& rhs) const { return !isEqual(rhs); }

        template<typename C>
        explicit operator Point2T<C>() const {
            return Point2T<C>(static_cast<C>(x), static_cast<C>(y));
        }

        Point2T& add(const Vector2T<T>& rhs) {
            x += rhs.x; y += rhs.y;
            return *this;
        }
        Point2T& sub(const Vector2T<T>& rhs) {
            x -= rhs.x; y -= rhs.y;
            return *this;
        }
        bool isEqual(const Point2T& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        void set(T _x, T _y) {
            x = _x; y = _y;
        }

        T x, y;
    };

    template<typename T>
    class Point3T {
    public:
        Point3T() : x(0), y(0), z(0) {}
        Point3T(T x, T y, T z) : x(x), y(y), z(z) {}
        Point3T(const Point2T<T>& p, T z) : x(p.x), y(p.y), z(z) {}

        Point3T operator+(const Vector3T<T>& rhs) const { return Point3T(*this).add(rhs); }
        Vector3T<T> operator-(const Point3T& rhs) const { return Vector3T<T>(x - rhs.x, y - rhs.y, z - rhs.z); }
        Point3T operator-(const Vector3T<T>& rhs) const { return Point3T(*this).sub(rhs); }
        Vector3T<T> toVector() const { return Vector3T<T>(x, y, z); }
        bool operator==(const Point3T& rhs) const { return isEqual(rhs); }
        bool operator!=(const Point3T& rhs) const { return !isEqual(rhs); }

        template<typename C>
        operator Point3T<C>() const {
            return Point3T<C>(x, y, z);
        }

        Point3T& add(const Vector3T<T>& rhs) {
            x += rhs.x; y += rhs.y; z += rhs.z;
            return *this;
        }
        Point3T& sub(const Vector3T<T>& rhs) {
            x -= rhs.x; y -= rhs.y; z -= rhs.z;
            return *this;
        }
        bool isEqual(const Point3T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        void set(T _x, T _y, T _z) {
            x = _x; y = _y; z = _z;
        }

        T x, y, z;
    };

    template<typename T>
    class Point4T {
    public:
        Point4T() : x(0), y(0), z(0), w(0) {}
        Point4T(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
        Point4T(const Point3T<T>& p, T w) : x(p.x), y(p.y), z(p.z), w(w) {}

        Point4T operator+(const Vector4T<T>& rhs) const { return Point4T(*this).add(rhs); }
        Vector4T<T> operator-(const Point4T& rhs) const { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
        Point4T operator-(const Vector4T<T>& rhs) const { return Point4T(*this).sub(rhs); }
        Vector4T<T> toVector() const { return Vector4T<T>(x, y, z, w); }
        bool operator==(const Point4T& rhs) const { return isEqual(rhs); }
        bool operator!=(const Point4T& rhs) const { return !isEqual(rhs); }

        template<typename C>
        explicit operator Point4T<C>() const {
            return Point4T<C>(x, y, z, w);
        }

        Point4T& add(const Vector4T<T>& rhs) {
            x += rhs.x; y += rhs.y;
            z += rhs.z; w += rhs.w;
            return *this;
        }
        Point4T& sub(const Vector4T<T>& rhs) {
            x -= rhs.x; y -= rhs.y;
            z -= rhs.z; w -= rhs.w;
            return *this;
        }
        bool isEqual(const Point4T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        void set(T _x, T _y, T _z, T _w) {
            x = _x; y = _y; z = _z; w = _w;
        }

        Point3T<T> p3() const { return Point3T<T>(x, y, z); }

        T x, y, z, w;
    };


    using Point2F = Point2T<float>;
    using Point3F = Point3T<float>;
    using Point4F = Point4T<float>;
    using Point2D = Point2T<double>;
    using Point3D = Point3T<double>;
    using Point4D = Point4T<double>;

}

#endif  // UKIVE_GRAPHICS_3D_POINT_H_