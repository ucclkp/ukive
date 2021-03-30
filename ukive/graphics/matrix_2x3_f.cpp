// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "matrix_2x3_f.h"

#include <cmath>


namespace ukive {

    Matrix2x3F::Matrix2x3F()
        : m11(1), m12(0), m13(0), m21(0), m22(1), m23(0) {}

    Matrix2x3F::Matrix2x3F(
        float m11, float m12, float m13,
        float m21, float m22, float m23)
        : m11(m11), m12(m12), m13(m13), m21(m21), m22(m22), m23(m23) {}

    Matrix2x3F Matrix2x3F::operator*(const Matrix2x3F& rhs) const {
        return Matrix2x3F().setMul(*this, rhs);
    }

    Matrix2x3F& Matrix2x3F::identity() {
        m11 = 1; m12 = 0; m13 = 0;
        m21 = 0; m22 = 1; m23 = 0;
        return *this;
    }

    Matrix2x3F& Matrix2x3F::mul(const Matrix2x3F& rhs) {
        *this = Matrix2x3F().setMul(*this, rhs);
        return *this;
    }

    Matrix2x3F& Matrix2x3F::setMul(const Matrix2x3F& lhs, const Matrix2x3F& rhs) {
        m11 = lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21;
        m12 = lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22;
        m13 = lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13;
        m21 = lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21;
        m22 = lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22;
        m23 = lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23;
        return *this;
    }

    float Matrix2x3F::get(Element e) const {
        switch (e) {
        case TRANS_X: return m13;
        case TRANS_Y: return m23;
        case SCALE_X: return m11;
        case SCALE_Y: return m22;
        }
        return 0.f;
    }

    void Matrix2x3F::preTranslate(float dx, float dy) {
        mul(translate(dx, dy));
    }

    void Matrix2x3F::postTranslate(float dx, float dy) {
        *this = translate(dx, dy).mul(*this);
    }

    void Matrix2x3F::preScale(float sx, float sy) {
        mul(scale(sx, sy));
    }

    void Matrix2x3F::postScale(float sx, float sy) {
        *this = scale(sx, sy).mul(*this);
    }

    void Matrix2x3F::preScale(float sx, float sy, float cx, float cy) {
        mul(translate(cx, cy) * scale(sx, sy) * translate(-cx, -cy));
    }

    void Matrix2x3F::postScale(float sx, float sy, float cx, float cy) {
        *this = (translate(cx, cy) * scale(sx, sy) * translate(-cx, -cy)).mul(*this);
    }

    void Matrix2x3F::preRotate(float angle) {
        mul(rotate(angle));
    }

    void Matrix2x3F::postRotate(float angle) {
        *this = rotate(angle).mul(*this);
    }

    void Matrix2x3F::preRotate(float angle, float cx, float cy) {
        mul(translate(cx, cy) * rotate(angle) * translate(-cx, -cy));
    }

    void Matrix2x3F::postRotate(float angle, float cx, float cy) {
        *this = (translate(cx, cy) * rotate(angle) * translate(-cx, -cy)).mul(*this);
    }

    void Matrix2x3F::transformPoint(PointF* point) {
        auto x = point->x*m11 + point->y*m12 + m13;
        auto y = point->x*m21 + point->y*m22 + m23;
        point->x = x;
        point->y = y;
    }

    void Matrix2x3F::transformRect(
        const RectF& rect, PointF* lt, PointF* tr, PointF* rb, PointF* bl)
    {
        PointF s_lt(rect.left, rect.top);
        PointF s_tr(rect.right, rect.top);
        PointF s_rb(rect.right, rect.bottom);
        PointF s_bl(rect.left, rect.bottom);

        transformPoint(&s_lt);
        transformPoint(&s_tr);
        transformPoint(&s_rb);
        transformPoint(&s_bl);

        *lt = s_lt;
        *tr = s_tr;
        *rb = s_rb;
        *bl = s_bl;
    }

    // static
    Matrix2x3F Matrix2x3F::scale(float sx, float sy) {
        return Matrix2x3F(
            sx, 0,  0,
            0,  sy, 0
        );
    }

    // static
    Matrix2x3F Matrix2x3F::shearX(float s) {
        return Matrix2x3F(
            1, s, 0,
            0, 1, 0
        );
    }

    // static
    Matrix2x3F Matrix2x3F::shearY(float s) {
        return Matrix2x3F(
            1, 0, 0,
            s, 1, 0
        );
    }

    // static
    Matrix2x3F Matrix2x3F::rotate(float angle) {
        double _angle = angle * 3.1415926535898 / 180;
        return Matrix2x3F(
            float(cos(_angle)), float(-sin(_angle)), 0,
            float(sin(_angle)), float(cos(_angle)),  0
        );
    }

    // static
    Matrix2x3F Matrix2x3F::reflect(bool rx, bool ry) {
        return Matrix2x3F(
            (ry ? -1.f : 1.f), 0,                 0,
            0,                 (rx ? -1.f : 1.f), 0
        );
    }

    // static
    Matrix2x3F Matrix2x3F::translate(float tx, float ty) {
        return Matrix2x3F(
            1, 0, tx,
            0, 1, ty
        );
    }

}
