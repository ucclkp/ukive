// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MATRIX_2X3_HPP_
#define UKIVE_GRAPHICS_MATRIX_2X3_HPP_

#include <cmath>

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    /**
     * 专用于 2D 变换的矩阵。
     * 为列优先矩阵：变换顺序为从右到左。
     */
    template <typename Ty>
    class Matrix2x3T {
    public:
        enum Element {
            TRANS_X,
            TRANS_Y,
            SCALE_X,
            SCALE_Y
        };

        Matrix2x3T()
            : m11(1), m12(0), m13(0), m21(0), m22(1), m23(0) {}
        Matrix2x3T(
            Ty m11, Ty m12, Ty m13,
            Ty m21, Ty m22, Ty m23)
            : m11(m11), m12(m12), m13(m13), m21(m21), m22(m22), m23(m23) {}

        Matrix2x3T operator*(const Matrix2x3T& rhs) const {
            return Matrix2x3T().setMul(*this, rhs);
        }

        Matrix2x3T& identity() {
            m11 = 1; m12 = 0; m13 = 0;
            m21 = 0; m22 = 1; m23 = 0;
            return *this;
        }

        Matrix2x3T& mul(const Matrix2x3T& rhs) {
            *this = Matrix2x3T().setMul(*this, rhs);
            return *this;
        }

        Matrix2x3T& setMul(const Matrix2x3T& lhs, const Matrix2x3T& rhs) {
            m11 = lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21;
            m12 = lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22;
            m13 = lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13;
            m21 = lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21;
            m22 = lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22;
            m23 = lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23;
            return *this;
        }

        Ty get(Element e) const {
            switch (e) {
            case TRANS_X: return m13;
            case TRANS_Y: return m23;
            case SCALE_X: return m11;
            case SCALE_Y: return m22;
            }
            return 0.f;
        }

        void preTranslate(Ty dx, Ty dy) {
            mul(translate(dx, dy));
        }
        void postTranslate(Ty dx, Ty dy) {
            *this = translate(dx, dy).mul(*this);
        }

        void preScale(Ty sx, Ty sy) {
            mul(scale(sx, sy));
        }
        void postScale(Ty sx, Ty sy) {
            *this = scale(sx, sy).mul(*this);
        }
        void preScale(Ty sx, Ty sy, Ty cx, Ty cy) {
            mul(translate(cx, cy) * scale(sx, sy) * translate(-cx, -cy));
        }
        void postScale(Ty sx, Ty sy, Ty cx, Ty cy) {
            *this = (translate(cx, cy) * scale(sx, sy) * translate(-cx, -cy)).mul(*this);
        }

        void preRotate(Ty angle) {
            mul(rotate(angle));
        }
        void postRotate(Ty angle) {
            *this = rotate(angle).mul(*this);
        }
        void preRotate(Ty angle, Ty cx, Ty cy) {
            mul(translate(cx, cy) * rotate(angle) * translate(-cx, -cy));
        }
        void postRotate(Ty angle, Ty cx, Ty cy) {
            *this = (translate(cx, cy) * rotate(angle) * translate(-cx, -cy)).mul(*this);
        }

        void transformPoint(PointT<Ty>* point) {
            auto x = point->x() * m11 + point->y() * m12 + m13;
            auto y = point->x() * m21 + point->y() * m22 + m23;
            point->x(x);
            point->y(y);
        }
        void transformRect(
            const RectT<Ty>& rect,
            PointT<Ty>* lt, PointT<Ty>* tr, PointT<Ty>* rb, PointT<Ty>* bl)
        {
            PointT<Ty> s_lt{ rect.x(), rect.y() };
            PointT<Ty> s_tr{ rect.right(), rect.y() };
            PointT<Ty> s_rb{ rect.right(), rect.bottom() };
            PointT<Ty> s_bl{ rect.x(), rect.bottom() };

            transformPoint(&s_lt);
            transformPoint(&s_tr);
            transformPoint(&s_rb);
            transformPoint(&s_bl);

            *lt = s_lt;
            *tr = s_tr;
            *rb = s_rb;
            *bl = s_bl;
        }

        static Matrix2x3T scale(Ty sx, Ty sy) {
            return Matrix2x3T(
                sx, 0, 0,
                0, sy, 0
            );
        }
        static Matrix2x3T shearX(Ty s) {
            return Matrix2x3T(
                1, s, 0,
                0, 1, 0
            );
        }
        static Matrix2x3T shearY(Ty s) {
            return Matrix2x3T(
                1, 0, 0,
                s, 1, 0
            );
        }
        static Matrix2x3T rotate(Ty angle) {
            if constexpr (std::is_same<Ty, float>::value) {
                double _angle = angle * 3.1415926535898 / 180;
                return Matrix2x3T(
                    Ty(cos(_angle)), Ty(-sin(_angle)), 0,
                    Ty(sin(_angle)), Ty(cos(_angle)), 0
                );
            } else {
                Ty _angle = angle * 3.1415926535898 / 180;
                return Matrix2x3T(
                    cos(_angle), -sin(_angle), 0,
                    sin(_angle), cos(_angle), 0
                );
            }
        }
        static Matrix2x3T reflect(bool rx, bool ry) {
            return Matrix2x3T(
                (ry ? -1 : 1), 0, 0,
                0, (rx ? -1 : 1), 0
            );
        }
        static Matrix2x3T translate(Ty tx, Ty ty) {
            return Matrix2x3T(
                1, 0, tx,
                0, 1, ty
            );
        }

        Ty m11, m12, m13,
           m21, m22, m23;
    };

    using Matrix2x3F = Matrix2x3T<float>;

}

#endif  // UKIVE_GRAPHICS_MATRIX_2X3_HPP_