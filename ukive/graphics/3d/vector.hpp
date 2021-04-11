// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_VECTOR_H_
#define UKIVE_GRAPHICS_3D_VECTOR_H_

#include <cmath>
#include <type_traits>


namespace ukv3d {

    template<typename Ty>
    class Vector2T {
    public:
        template <typename Y>
        class CheckingStub : public std::false_type {};

        Vector2T() : x(0), y(0) {}
        Vector2T(Ty x, Ty y) : x(x), y(y) {}

        Vector2T operator+(const Vector2T& rhs) const {
            return Vector2T(*this).add(rhs);
        }
        Vector2T operator-(const Vector2T& rhs) const {
            return Vector2T(*this).sub(rhs);
        }
        Vector2T operator-() const {
            return Vector2T(*this).inv();
        }
        Vector2T operator*(Ty factor) const {
            return Vector2T(*this).mul(factor);
        }
        Ty operator*(const Vector2T& rhs) const {
            return Vector2T(*this).mul(rhs);
        }
        bool operator==(const Vector2T& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const Vector2T& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator Vector2T<C>() const {
            return Vector2T<C>(x, y);
        }

        Vector2T operator/(Ty factor) const {
            return Vector2T(*this).div(factor);
        }

        Vector2T normalize() const {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            return Vector2T(*this).nor();
        }

        Vector2T& add(const Vector2T& rhs) {
            x += rhs.x; y += rhs.y;
            return *this;
        }

        Vector2T& sub(const Vector2T& rhs) {
            x -= rhs.x; y -= rhs.y;
            return *this;
        }

        Vector2T& mul(Ty factor) {
            x *= factor; y *= factor;
            return *this;
        }

        Vector2T& inv() {
            x = -x; y = -y;
            return *this;
        }

        Ty mul(const Vector2T& rhs) {
            return x * rhs.x + y * rhs.y;
        }

        Ty lengthSq() const {
            return x*x + y*y;
        }

        bool equal(const Vector2T& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        Vector2T& div(Ty factor) {
            x /= factor; y /= factor;
            return *this;
        }

        Vector2T& nor() {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            auto l = length();
            x /= l; y /= l;
            return *this;
        }

        Ty length() const {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            return sqrt(x*x + y*y);
        }

        void set(Ty _x, Ty _y) {
            x = _x;
            y = _y;
        }

        Ty x, y;
    };


    template<typename Ty>
    class Vector3T {
    public:
        template <typename Y>
        class CheckingStub : public std::false_type {};

        Vector3T() : x(0), y(0), z(0) {}
        Vector3T(Ty x, Ty y, Ty z) : x(x), y(y), z(z) {}

        Vector3T operator+(const Vector3T& rhs) const {
            return Vector3T(*this).add(rhs);
        }
        Vector3T operator-(const Vector3T& rhs) const {
            return Vector3T(*this).sub(rhs);
        }
        Vector3T operator-() const {
            return Vector3T(*this).inv();
        }
        Vector3T operator*(Ty factor) const {
            return Vector3T(*this).mul(factor);
        }
        Ty operator*(const Vector3T& rhs) const {
            return Vector3T(*this).mul(rhs);
        }
        Vector3T operator^(const Vector3T& rhs) const {
            return Vector3T(*this).cross(rhs);
        }
        bool operator==(const Vector3T& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const Vector3T& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator Vector3T<C>() const {
            return Vector3T<C>(
                static_cast<C>(x),
                static_cast<C>(y),
                static_cast<C>(z));
        }

        Vector3T operator/(Ty factor) const {
            return Vector3T(*this).div(factor);
        }

        Vector3T normalize() const {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            return Vector3T(*this).nor();
        }

        Vector3T& add(const Vector3T& rhs) {
            x += rhs.x; y += rhs.y; z += rhs.z;
            return *this;
        }

        Vector3T& sub(const Vector3T& rhs) {
            x -= rhs.x; y -= rhs.y; z -= rhs.z;
            return *this;
        }

        Vector3T& mul(Ty factor) {
            x *= factor; y *= factor; z *= factor;
            return *this;
        }

        Vector3T& inv() {
            x = -x; y = -y; z = -z;
            return *this;
        }

        Vector3T& cross(const Vector3T& rhs) {
            auto _x = y * rhs.z - z * rhs.y;
            auto _y = z * rhs.x - x * rhs.z;
            auto _z = x * rhs.y - y * rhs.x;
            x = _x; y = _y; z = _z;
            return *this;
        }

        Ty mul(const Vector3T& rhs) {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        Ty lengthSq() const {
            return x*x + y*y + z*z;
        }

        bool equal(const Vector3T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        Vector3T& div(Ty factor) {
            x /= factor; y /= factor; z /= factor;
            return *this;
        }

        Vector3T& nor() {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            auto l = length();
            x /= l; y /= l; z /= l;
            return *this;
        }

        Ty length() const {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            return sqrt(x*x + y*y + z*z);
        }

        void set(Ty _x, Ty _y, Ty _z) {
            x = _x;
            y = _y;
            z = _z;
        }

        Ty x, y, z;
    };


    template<typename Ty>
    class Matrix4x4T;

    template<typename Ty>
    class Vector4T {
    public:
        template <typename Y>
        class CheckingStub : public std::false_type {};

        Vector4T() : x(0), y(0), z(0), w(0) {}
        Vector4T(Ty x, Ty y, Ty z, Ty w) : x(x), y(y), z(z), w(w) {}

        Vector4T operator+(const Vector4T& rhs) const {
            return Vector4T(*this).add(rhs);
        }
        Vector4T operator-(const Vector4T& rhs) const {
            return Vector4T(*this).sub(rhs);
        }
        Vector4T operator-() const {
            return Vector4T(*this).inv();
        }
        Vector4T operator*(Ty factor) const {
            return Vector4T(*this).mul(factor);
        }
        Vector4T operator*(const Matrix4x4T<Ty>& m) const {
            return Vector4T(*this).mul(m);
        }
        Ty operator*(const Vector4T& rhs) const {
            return Vector4T(*this).mul(rhs);
        }
        bool operator==(const Vector4T& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const Vector4T& rhs) const {
            return !equal(rhs);
        }

        template<typename C>
        explicit operator Vector4T<C>() const {
            return Vector4T<C>(x, y, z, w);
        }

        Vector4T operator/(Ty factor) const {
            return Vector4T(*this).div(factor);
        }

        Vector4T normalize() const {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            return Vector4T(*this).nor();
        }

        Vector4T& add(const Vector4T& rhs) {
            x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
            return *this;
        }

        Vector4T& sub(const Vector4T& rhs) {
            x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
            return *this;
        }

        Vector4T& mul(Ty factor) {
            x *= factor; y *= factor; z *= factor; w *= factor;
            return *this;
        }

        Vector4T& inv() {
            x = -x; y = -y; z = -z; w = -w;
            return *this;
        }

        Ty mul(const Vector4T& rhs) {
            return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
        }

        Vector4T& mul(const Matrix4x4T<Ty>& m) {
            auto left = *this;
            x = left.x * m.m11 + left.y * m.m21 + left.z * m.m31 + left.w * m.m41;
            y = left.x * m.m12 + left.y * m.m22 + left.z * m.m32 + left.w * m.m42;
            z = left.x * m.m13 + left.y * m.m23 + left.z * m.m33 + left.w * m.m43;
            w = left.x * m.m14 + left.y * m.m24 + left.z * m.m34 + left.w * m.m44;
            return *this;
        }

        Ty lengthSq() const {
            return x * x + y * y + z * z + w * w;
        }

        bool equal(const Vector4T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        Vector4T& div(Ty factor) {
            x /= factor; y /= factor; z /= factor; w /= factor;
            return *this;
        }

        Vector4T& nor() {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            auto l = length();
            x /= l; y /= l; z /= l; w /= l;
            return *this;
        }

        Ty length() const {
            if constexpr (!std::is_floating_point<Ty>::value) {
                static_assert(CheckingStub<Ty>::value, "Ty must be a floating type!");
            }
            return sqrt(x*x + y * y + z * z + w * w);
        }

        Vector3T<Ty> v3() const {
            return Vector3T<Ty>(x, y, z);
        }

        void set(Ty _x, Ty _y, Ty _z, Ty _w) {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }

        Ty x, y, z, w;
    };


    using Vector2F = Vector2T<float>;
    using Vector3F = Vector3T<float>;
    using Vector4F = Vector4T<float>;
    using Vector2D = Vector2T<double>;
    using Vector3D = Vector3T<double>;
    using Vector4D = Vector4T<double>;

}

#endif  // UKIVE_GRAPHICS_3D_VECTOR_H_