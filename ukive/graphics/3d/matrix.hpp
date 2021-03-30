// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_MATRIX_H_
#define UKIVE_GRAPHICS_3D_MATRIX_H_

#include "point.hpp"
#include "vector.hpp"

#include <limits>


namespace ukv3d {

    template <typename Ty>
    class Matrix3x3T {
    public:
        Matrix3x3T()
            : m11(0), m12(0), m13(0),
              m21(0), m22(0), m23(0),
              m31(0), m32(0), m33(0) {}
        Matrix3x3T(
            Ty m11, Ty m12, Ty m13,
            Ty m21, Ty m22, Ty m23,
            Ty m31, Ty m32, Ty m33)
            : m11(m11), m12(m12), m13(m13),
              m21(m21), m22(m22), m23(m23),
              m31(m31), m32(m32), m33(m33) {}
        Matrix3x3T(const Matrix3x3T& rhs)
            : m11(rhs.m11), m12(rhs.m12), m13(rhs.m13),
              m21(rhs.m21), m22(rhs.m22), m23(rhs.m23),
              m31(rhs.m31), m32(rhs.m32), m33(rhs.m33) {}

        Matrix3x3T& operator=(const Matrix3x3T& rhs) {
            m11 = rhs.m11; m12 = rhs.m12; m13 = rhs.m13;
            m21 = rhs.m21; m22 = rhs.m22; m23 = rhs.m23;
            m31 = rhs.m31; m32 = rhs.m32; m33 = rhs.m33;
            return *this;
        }

        Matrix3x3T operator+(const Matrix3x3T& rhs) {
            Matrix3x3T m(*this);
            m.add(rhs);
            return m;
        }

        Matrix3x3T operator-(const Matrix3x3T& rhs) {
            Matrix3x3T m(*this);
            m.sub(rhs);
            return m;
        }

        Matrix3x3T operator*(const Matrix3x3T& rhs) {
            Matrix3x3T m(*this);
            m.mul(rhs);
            return m;
        }

        Point2T<Ty> operator*(const Point2T<Ty>& p) {
            return {
                m11 * p.x + m12 * p.y + m13,
                m21 * p.x + m22 * p.y + m23
            };
        }

        Point3T<Ty> operator*(const Point3T<Ty>& p) {
            return {
                m11 * p.x + m12 * p.y + m13 * p.z,
                m21 * p.x + m22 * p.y + m23 * p.z,
                m31 * p.x + m32 * p.y + m33 * p.z
            };
        }

        Vector3T<Ty> operator*(const Vector3T<Ty>& v) {
            return {
                m11 * v.x + m12 * v.y + m13 * v.z,
                m21 * v.x + m22 * v.y + m23 * v.z,
                m31 * v.x + m32 * v.y + m33 * v.z
            };
        }

        Matrix3x3T operator*(Ty factor) {
            Matrix3x3T m(*this);
            m.mul(factor);
            return m;
        }

        Matrix3x3T operator/(Ty factor) {
            Matrix3x3T m(*this);
            m.div(factor);
            return m;
        }

        template<typename C>
        explicit operator Matrix3x3T<C>() const {
            return Matrix3x3T<C>(
                static_cast<C>(m11), static_cast<C>(m12), static_cast<C>(m13),
                static_cast<C>(m21), static_cast<C>(m22), static_cast<C>(m23),
                static_cast<C>(m31), static_cast<C>(m32), static_cast<C>(m33));
        }

        void identity() {
            m11 = 1; m12 = 0; m13 = 0;
            m21 = 0; m22 = 1; m23 = 0;
            m31 = 0; m32 = 0; m33 = 1;
        }

        /**
         * 转置矩阵。
         * 结果输出至 out。注意 out 不能是 this。
         */
        void transport(Matrix3x3T* out) const {
            out->m11 = m11;
            out->m12 = m21;
            out->m13 = m31;

            out->m21 = m12;
            out->m22 = m22;
            out->m23 = m32;

            out->m31 = m13;
            out->m32 = m23;
            out->m33 = m33;
        }

        /**
         * 求可逆矩阵。
         * 结果输出至 out。注意 out 不能是 this。
         * 如果逆矩阵存在，返回 true，否则返回 false。
         */
        bool inverse(Matrix3x3T* out) const {
            auto sigma = m11*(m22*m33 - m23*m32) - m12*(m21*m33 - m23*m31) + m13*(m21*m32 - m22*m31);
            if (std::abs(sigma) <= std::numeric_limits<Ty>::epsilon() * std::abs(sigma) ||
                std::abs(sigma) < std::numeric_limits<Ty>::min())
            {
                return false;
            }

            out->m11 =  (m22*m33 - m23*m32) / sigma;
            out->m12 = -(m12*m33 - m13*m32) / sigma;
            out->m13 =  (m12*m23 - m13*m22) / sigma;

            out->m21 = -(m21*m33 - m23*m31) / sigma;
            out->m22 =  (m11*m33 - m13*m31) / sigma;
            out->m23 = -(m11*m23 - m13*m21) / sigma;

            out->m31 =  (m21*m32 - m22*m31) / sigma;
            out->m32 = -(m11*m32 - m12*m31) / sigma;
            out->m33 =  (m11*m22 - m12*m21) / sigma;

            return true;
        }

        void add(const Matrix3x3T& rhs) {
            m11 += rhs.m11; m12 += rhs.m12; m13 += rhs.m13;
            m21 += rhs.m21; m22 += rhs.m22; m23 += rhs.m23;
            m31 += rhs.m31; m32 += rhs.m32; m33 += rhs.m33;
        }

        void sub(const Matrix3x3T& rhs) {
            m11 -= rhs.m11; m12 -= rhs.m12; m13 -= rhs.m13;
            m21 -= rhs.m21; m22 -= rhs.m22; m23 -= rhs.m23;
            m31 -= rhs.m31; m32 -= rhs.m32; m33 -= rhs.m33;
        }

        void mul(const Matrix3x3T& rhs) {
            auto _m11 = m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31;
            auto _m12 = m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32;
            auto _m13 = m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33;

            auto _m21 = m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31;
            auto _m22 = m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32;
            auto _m23 = m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33;

            auto _m31 = m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31;
            auto _m32 = m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32;
            auto _m33 = m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33;

            m11 = _m11; m12 = _m12; m13 = _m13;
            m21 = _m21; m22 = _m22; m23 = _m23;
            m31 = _m31; m32 = _m32; m33 = _m33;
        }

        void mul(Ty factor) {
            m11 *= factor; m12 *= factor; m13 *= factor;
            m21 *= factor; m22 *= factor; m23 *= factor;
            m31 *= factor; m32 *= factor; m33 *= factor;
        }

        void div(Ty factor) {
            m11 /= factor; m12 /= factor; m13 /= factor;
            m21 /= factor; m22 /= factor; m23 /= factor;
            m31 /= factor; m32 /= factor; m33 /= factor;
        }

        /**
         * 构造缩放矩阵。
         */
        static Matrix3x3T scale(Ty sx, Ty sy) {
            return {
                sx, 0,  0,
                0,  sy, 0,
                0,  0,  1
            };
        }

        /**
         * 构造旋转矩阵。
         * 注意：构造的矩阵为列优先矩阵。
         */
        static Matrix3x3T rotate(Ty angle) {
            return {
                cos(angle), -sin(angle),  0,
                sin(angle),  cos(angle),  0,
                0,          0,            1
            };
        }

        /**
         * 构造平移矩阵。
         * 注意：构造的矩阵为列优先矩阵。
         */
        static Matrix3x3T translate(Ty tx, Ty ty) {
            return {
                1, 0, tx,
                0, 1, ty,
                0, 0, 1
            };
        }

        Ty m11, m12, m13,
            m21, m22, m23,
            m31, m32, m33;
    };


    template <typename Ty>
    class Matrix4x4T {
    public:
        Matrix4x4T()
            : m11(0), m12(0), m13(0), m14(0),
              m21(0), m22(0), m23(0), m24(0),
              m31(0), m32(0), m33(0), m34(0),
              m41(0), m42(0), m43(0), m44(0) {}
        Matrix4x4T(
            Ty m11, Ty m12, Ty m13, Ty m14,
            Ty m21, Ty m22, Ty m23, Ty m24,
            Ty m31, Ty m32, Ty m33, Ty m34,
            Ty m41, Ty m42, Ty m43, Ty m44)
            : m11(m11), m12(m12), m13(m13), m14(m14),
              m21(m21), m22(m22), m23(m23), m24(m24),
              m31(m31), m32(m32), m33(m33), m34(m34),
              m41(m41), m42(m42), m43(m43), m44(m44) {}
        Matrix4x4T(const Matrix4x4T& rhs)
            : m11(rhs.m11), m12(rhs.m12), m13(rhs.m13), m14(rhs.m14),
              m21(rhs.m21), m22(rhs.m22), m23(rhs.m23), m24(rhs.m24),
              m31(rhs.m31), m32(rhs.m32), m33(rhs.m33), m34(rhs.m34),
              m41(rhs.m41), m42(rhs.m42), m43(rhs.m43), m44(rhs.m44) {}

        Matrix4x4T& operator=(const Matrix4x4T& rhs) {
            m11 = rhs.m11; m12 = rhs.m12; m13 = rhs.m13; m14 = rhs.m14;
            m21 = rhs.m21; m22 = rhs.m22; m23 = rhs.m23; m24 = rhs.m24;
            m31 = rhs.m31; m32 = rhs.m32; m33 = rhs.m33; m34 = rhs.m34;
            m41 = rhs.m41; m42 = rhs.m42; m43 = rhs.m43; m44 = rhs.m44;
            return *this;
        }

        Matrix4x4T operator+(const Matrix4x4T& rhs) {
            Matrix4x4T m(*this);
            m.add(rhs);
            return m;
        }

        Matrix4x4T operator-(const Matrix4x4T& rhs) {
            Matrix4x4T m(*this);
            m.sub(rhs);
            return m;
        }

        Matrix4x4T operator*(const Matrix4x4T& rhs) {
            Matrix4x4T m(*this);
            m.mul(rhs);
            return m;
        }

        Point4T<Ty> operator*(const Point4T<Ty>& p) {
            return {
                m11 * p.x + m12 * p.y + m13 * p.z + m14 * p.w,
                m21 * p.x + m22 * p.y + m23 * p.z + m24 * p.w,
                m31 * p.x + m32 * p.y + m33 * p.z + m34 * p.w,
                m41 * p.x + m42 * p.y + m43 * p.z + m44 * p.w
            };
        }

        Vector4T<Ty> operator*(const Vector4T<Ty>& v) {
            return {
                m11 * v.x + m12 * v.y + m13 * v.z + m14 * v.w,
                m21 * v.x + m22 * v.y + m23 * v.z + m24 * v.w,
                m31 * v.x + m32 * v.y + m33 * v.z + m34 * v.w,
                m41 * v.x + m42 * v.y + m43 * v.z + m44 * v.w
            };
        }

        Matrix4x4T operator*(Ty factor) {
            Matrix4x4T m(*this);
            m.mul(factor);
            return m;
        }

        Matrix4x4T operator/(Ty factor) {
            Matrix4x4T m(*this);
            m.div(factor);
            return m;
        }

        template<typename C>
        explicit operator Matrix4x4T<C>() const {
            return Matrix4x4T<C>(
                static_cast<C>(m11), static_cast<C>(m12), static_cast<C>(m13), static_cast<C>(m14),
                static_cast<C>(m21), static_cast<C>(m22), static_cast<C>(m23), static_cast<C>(m24),
                static_cast<C>(m31), static_cast<C>(m32), static_cast<C>(m33), static_cast<C>(m34),
                static_cast<C>(m41), static_cast<C>(m42), static_cast<C>(m43), static_cast<C>(m44));
        }

        void identity() {
            m11 = 1; m12 = 0; m13 = 0; m14 = 0;
            m21 = 0; m22 = 1; m23 = 0; m24 = 0;
            m31 = 0; m32 = 0; m33 = 1; m34 = 0;
            m41 = 0; m42 = 0; m43 = 0; m44 = 1;
        }

        /**
         * 转置矩阵。
         * 结果输出至 out。注意 out 不能是 this。
         */
        void transport(Matrix4x4T* out) const {
            out->m11 = m11;
            out->m12 = m21;
            out->m13 = m31;
            out->m14 = m41;

            out->m21 = m12;
            out->m22 = m22;
            out->m23 = m32;
            out->m24 = m42;

            out->m31 = m13;
            out->m32 = m23;
            out->m33 = m33;
            out->m34 = m43;

            out->m41 = m14;
            out->m42 = m24;
            out->m43 = m34;
            out->m44 = m44;
        }

        /**
         * 求可逆矩阵。
         * 结果输出至 out。注意 out 不能是 this。
         * 如果逆矩阵存在，返回 true，否则返回 false。
         */
        bool inverse(Matrix4x4T* out) const {
            auto sigma =
                  m11*m22*(m33*m44 - m34*m43) - m11*m23*(m32*m44 - m34*m42) + m11*m24*(m32*m43 - m33*m42)
                - m12*m21*(m33*m44 - m34*m43) + m12*m23*(m31*m44 - m34*m41) - m12*m24*(m31*m43 - m33*m41)
                + m13*m21*(m32*m44 - m34*m42) - m13*m22*(m31*m44 - m34*m41) + m13*m24*(m31*m42 - m32*m41)
                - m14*m21*(m32*m43 - m33*m42) + m14*m22*(m31*m43 - m33*m41) - m14*m23*(m31*m42 - m32*m41);
            if (std::abs(sigma) <= std::numeric_limits<Ty>::epsilon() * std::abs(sigma) ||
                std::abs(sigma) < std::numeric_limits<Ty>::min())
            {
                return false;
            }

            out->m11 =  (m22*(m33*m44 - m34*m43) - m23*(m32*m44 - m34*m42) + m24*(m32*m43 - m33*m42)) / sigma;
            out->m12 = -(m12*(m33*m44 - m34*m43) - m13*(m32*m44 - m34*m42) + m14*(m32*m43 - m33*m42)) / sigma;
            out->m13 =  (m12*(m23*m44 - m24*m43) - m13*(m22*m44 - m24*m42) + m14*(m22*m43 - m23*m42)) / sigma;
            out->m14 = -(m12*(m23*m34 - m24*m33) - m13*(m22*m34 - m24*m32) + m14*(m22*m33 - m23*m32)) / sigma;

            out->m21 = -(m21*(m33*m44 - m34*m43) - m23*(m31*m44 - m34*m41) + m24*(m31*m43 - m33*m41)) / sigma;
            out->m22 =  (m11*(m33*m44 - m34*m43) - m13*(m31*m44 - m34*m41) + m14*(m31*m43 - m33*m41)) / sigma;
            out->m23 = -(m11*(m23*m44 - m24*m43) - m13*(m21*m44 - m24*m41) + m14*(m21*m43 - m23*m41)) / sigma;
            out->m24 =  (m11*(m23*m34 - m24*m33) - m13*(m21*m34 - m24*m31) + m14*(m21*m33 - m23*m31)) / sigma;

            out->m31 =  (m21*(m32*m44 - m34*m42) - m22*(m31*m44 - m34*m41) + m24*(m31*m42 - m32*m41)) / sigma;
            out->m32 = -(m11*(m32*m44 - m34*m42) - m12*(m31*m44 - m34*m41) + m14*(m31*m42 - m32*m41)) / sigma;
            out->m33 =  (m11*(m22*m44 - m24*m42) - m12*(m21*m44 - m24*m41) + m14*(m21*m42 - m22*m41)) / sigma;
            out->m34 = -(m11*(m22*m34 - m24*m32) - m12*(m21*m34 - m24*m31) + m14*(m21*m32 - m22*m31)) / sigma;

            out->m41 = -(m21*(m32*m43 - m33*m42) - m22*(m31*m43 - m33*m41) + m23*(m31*m42 - m32*m41)) / sigma;
            out->m42 =  (m11*(m32*m43 - m33*m42) - m12*(m31*m43 - m33*m41) + m13*(m31*m42 - m32*m41)) / sigma;
            out->m43 = -(m11*(m22*m43 - m23*m42) - m12*(m21*m43 - m23*m41) + m13*(m21*m42 - m22*m41)) / sigma;
            out->m44 =  (m11*(m22*m33 - m23*m32) - m12*(m21*m33 - m23*m31) + m13*(m21*m32 - m22*m31)) / sigma;

            return true;
        }

        void add(const Matrix4x4T& rhs) {
            m11 += rhs.m11; m12 += rhs.m12; m13 += rhs.m13; m14 += rhs.m14;
            m21 += rhs.m21; m22 += rhs.m22; m23 += rhs.m23; m24 += rhs.m24;
            m31 += rhs.m31; m32 += rhs.m32; m33 += rhs.m33; m34 += rhs.m34;
            m41 += rhs.m41; m42 += rhs.m42; m43 += rhs.m43; m44 += rhs.m44;
        }

        void sub(const Matrix4x4T& rhs) {
            m11 -= rhs.m11; m12 -= rhs.m12; m13 -= rhs.m13; m14 -= rhs.m14;
            m21 -= rhs.m21; m22 -= rhs.m22; m23 -= rhs.m23; m24 -= rhs.m24;
            m31 -= rhs.m31; m32 -= rhs.m32; m33 -= rhs.m33; m34 -= rhs.m34;
            m41 -= rhs.m41; m42 -= rhs.m42; m43 -= rhs.m43; m44 -= rhs.m44;
        }

        void mul(const Matrix4x4T& rhs) {
            auto _m11 = m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31 + m14 * rhs.m41;
            auto _m12 = m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32 + m14 * rhs.m42;
            auto _m13 = m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33 + m14 * rhs.m43;
            auto _m14 = m11 * rhs.m14 + m12 * rhs.m24 + m13 * rhs.m34 + m14 * rhs.m44;

            auto _m21 = m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31 + m24 * rhs.m41;
            auto _m22 = m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32 + m24 * rhs.m42;
            auto _m23 = m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33 + m24 * rhs.m43;
            auto _m24 = m21 * rhs.m14 + m22 * rhs.m24 + m23 * rhs.m34 + m24 * rhs.m44;

            auto _m31 = m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31 + m34 * rhs.m41;
            auto _m32 = m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32 + m34 * rhs.m42;
            auto _m33 = m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33 + m34 * rhs.m43;
            auto _m34 = m31 * rhs.m14 + m32 * rhs.m24 + m33 * rhs.m34 + m34 * rhs.m44;

            auto _m41 = m41 * rhs.m11 + m42 * rhs.m21 + m43 * rhs.m31 + m44 * rhs.m41;
            auto _m42 = m41 * rhs.m12 + m42 * rhs.m22 + m43 * rhs.m32 + m44 * rhs.m42;
            auto _m43 = m41 * rhs.m13 + m42 * rhs.m23 + m43 * rhs.m33 + m44 * rhs.m43;
            auto _m44 = m41 * rhs.m14 + m42 * rhs.m24 + m43 * rhs.m34 + m44 * rhs.m44;

            m11 = _m11; m12 = _m12; m13 = _m13; m14 = _m14;
            m21 = _m21; m22 = _m22; m23 = _m23; m24 = _m24;
            m31 = _m31; m32 = _m32; m33 = _m33; m34 = _m34;
            m41 = _m41; m42 = _m42; m43 = _m43; m44 = _m44;
        }

        void mul(Ty factor) {
            m11 *= factor; m12 *= factor; m13 *= factor; m14 *= factor;
            m21 *= factor; m22 *= factor; m23 *= factor; m24 *= factor;
            m31 *= factor; m32 *= factor; m33 *= factor; m34 *= factor;
            m41 *= factor; m42 *= factor; m43 *= factor; m44 *= factor;
        }

        void div(Ty factor) {
            m11 /= factor; m12 /= factor; m13 /= factor; m14 /= factor;
            m21 /= factor; m22 /= factor; m23 /= factor; m24 /= factor;
            m31 /= factor; m32 /= factor; m33 /= factor; m34 /= factor;
            m41 /= factor; m42 /= factor; m43 /= factor; m44 /= factor;
        }

        /**
         * 构造缩放矩阵。
         */
        static Matrix4x4T scale(Ty sx, Ty sy, Ty sz) {
            return {
                sx, 0,  0,  0,
                0,  sy, 0,  0,
                0,  0,  sz, 0,
                0,  0,  0,  1
            };
        }

        /**
         * 构造以 X 轴为中心的旋转矩阵。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T rotateX(Ty angle) {
            return {
                1, 0,          0,           0,
                0, cos(angle), -sin(angle), 0,
                0, sin(angle),  cos(angle), 0,
                0, 0,          0,           1
            };
        }

        /**
         * 构造以 Y 轴为中心的旋转矩阵。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T rotateY(Ty angle) {
            return {
                cos(angle),  0, sin(angle), 0,
                0,           1, 0,          0,
                -sin(angle), 0, cos(angle), 0,
                0,           0, 0,          1
            };
        }

        /**
         * 构造以 Z 轴为中心的旋转矩阵。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T rotateZ(Ty angle) {
            return {
                cos(angle), -sin(angle), 0, 0,
                sin(angle),  cos(angle), 0, 0,
                0,          0,           1, 0,
                0,          0,           0, 1
            };
        }

        /**
         * 构造以指定轴为中心的旋转矩阵。
         * 指定的轴为 axis，该轴过原点。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T rotateAxis(const Vector3T<Ty>& axis, Ty angle) {
            auto na = axis.normalize();

            auto sinc = std::sin(angle);
            auto cosc = std::cos(angle);
            auto x = axis.x;
            auto y = axis.y;
            auto z = axis.z;
            auto x2 = x*x;
            auto y2 = y*y;
            auto xy = x*y;
            auto xz = x*z;
            auto yz = y*z;

            return Matrix4x4T(
                     cosc - x2 * (cosc - 1), -z * sinc - xy * (cosc - 1),    y*sinc - xz * (cosc - 1), 0,
                   z*sinc - xy * (cosc - 1),        y2 - cosc * (y2 - 1), -x * sinc - yz * (cosc - 1), 0,
                -y * sinc - xz * (cosc - 1),    x*sinc - yz * (cosc - 1),    (cosc - 1)*(x2 + y2) + 1, 0,
                                          0,                           0,                           0, 1);
        }

        /**
         * 构造平移矩阵。
         * 注意：构造的矩阵为列优先矩阵。
         */
        static Matrix4x4T translate(Ty tx, Ty ty, Ty tz) {
            return {
                1, 0, 0, tx,
                0, 1, 0, ty,
                0, 0, 1, tz,
                0, 0, 0, 1
            };
        }

        /**
         * 构造视图矩阵。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T viewport(int pixel_width, int pixel_height) {
            return {
                pixel_width / 2.0, 0,                  0, (pixel_width - 1) / 2.0,
                0,                 pixel_height / 2.0, 0, (pixel_height - 1) / 2.0,
                0,                 0,                  1, 0,
                0,                 0,                  0, 1
            };
        }

        /**
         * 构造正规化矩阵。
         * 该矩阵将指定的位置 [l, t, r, b, n, f] 正规化为 [-1, -1, 1, 1, -1, 1]，
         * 并将正规空间的中心平移至原点。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T orthoProj(Ty l, Ty r, Ty b, Ty t, Ty n, Ty f) {
            return {
                2 / (r - l), 0,           0,           -(r + l) / (r - l),
                0,           2 / (t - b), 0,           -(t + b) / (t - b),
                0,           0,           2 / (n - f), -(n + f) / (n - f),
                0,           0,           0,           1
            };
        }

        /**
         * 构造投影矩阵。
         * 该矩阵仅进行缩放操作，请配合正规矩阵使用。
         * 另外，最终的变换结果请除以相应点的 W 分量。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T persp(Ty n, Ty f) {
            return {
                n, 0, 0,     0,
                0, n, 0,     0,
                0, 0, n + f, -f * n,
                0, 0, 1,     0
            };
        }

        /**
         * 构造摄像机矩阵。
         * 该矩阵将摄像机平移至原点，朝向 Z 轴负方向。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T camera(
            const Point3T<Ty>& eye, const Vector3T<Ty>& look, const Vector3T<Ty>& up)
        {
            auto w = -look.normalize();
            auto u = (up ^ w).normalize();
            auto v = w ^ u;
            return {
                u.x, u.y, u.z, -u.x*eye.x - u.y*eye.y - u.z*eye.z,
                v.x, v.y, v.z, -v.x*eye.x - v.y*eye.y - v.z*eye.z,
                w.x, w.y, w.z, -w.x*eye.x - w.y*eye.y - w.z*eye.z,
                0,    0,    0,    1
            };
        }

        /**
         * 构造摄像机逆矩阵。
         * 注意：构造的矩阵为右手列优先矩阵。
         */
        static Matrix4x4T cameraInverse(
            const Point3T<Ty>& eye, const Vector3T<Ty>& look, const Vector3T<Ty>& up)
        {
            auto w = -look.normalize();
            auto u = (up ^ w).normalize();
            auto v = w ^ u;
            return {
                u.x, v.x, w.x, eye.x,
                u.y, v.y, w.y, eye.y,
                u.z, v.z, w.z, eye.z,
                0,    0,    0,    1
            };
        }

        Ty m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44;
    };


    using Matrix3x3F = Matrix3x3T<float>;
    using Matrix4x4F = Matrix4x4T<float>;
    using Matrix3x3D = Matrix3x3T<double>;
    using Matrix4x4D = Matrix4x4T<double>;

}

#endif  // UKIVE_GRAPHICS_3D_MATRIX_H_