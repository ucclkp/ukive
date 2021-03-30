// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MATRIX_2X3_F_H_
#define UKIVE_GRAPHICS_MATRIX_2X3_F_H_

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    // 列优先矩阵
    // 变换顺序为从右到左
    class Matrix2x3F {
    public:
        enum Element {
            TRANS_X,
            TRANS_Y,
            SCALE_X,
            SCALE_Y
        };

        Matrix2x3F();
        Matrix2x3F(
            float m11, float m12, float m13,
            float m21, float m22, float m23);

        Matrix2x3F operator*(const Matrix2x3F& rhs) const;

        Matrix2x3F& identity();
        Matrix2x3F& mul(const Matrix2x3F& rhs);
        Matrix2x3F& setMul(const Matrix2x3F& lhs, const Matrix2x3F& rhs);

        float get(Element e) const;

        void preTranslate(float dx, float dy);
        void postTranslate(float dx, float dy);

        void preScale(float sx, float sy);
        void postScale(float sx, float sy);
        void preScale(float sx, float sy, float cx, float cy);
        void postScale(float sx, float sy, float cx, float cy);

        void preRotate(float angle);
        void postRotate(float angle);
        void preRotate(float angle, float cx, float cy);
        void postRotate(float angle, float cx, float cy);

        void transformPoint(PointF* point);
        void transformRect(
            const RectF& rect, PointF* lt, PointF* tr, PointF* rb, PointF* bl);

        static Matrix2x3F scale(float sx, float sy);
        static Matrix2x3F shearX(float s);
        static Matrix2x3F shearY(float s);
        static Matrix2x3F rotate(float angle);
        static Matrix2x3F reflect(bool rx, bool ry);
        static Matrix2x3F translate(float tx, float ty);

        float m11, m12, m13,
              m21, m22, m23;
    };

}

#endif  // UKIVE_GRAPHICS_MATRIX_2X3_F_H_