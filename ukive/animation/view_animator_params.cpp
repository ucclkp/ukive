// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/view_animator_params.h"

#include "ukive/graphics/matrix_2x3_f.h"


namespace ukive {

    ViewAnimatorParams::ViewAnimatorParams() {}

    void ViewAnimatorParams::setAlpha(double alpha) {
        alpha_ = alpha;
    }

    void ViewAnimatorParams::setScaleX(double sx) {
        scale_x_ = sx;
    }

    void ViewAnimatorParams::setScaleY(double sy) {
        scale_y_ = sy;
    }

    void ViewAnimatorParams::setRotate(double rotate) {
        rotate_ = rotate;
    }

    void ViewAnimatorParams::setTranslateX(double tx) {
        translate_x_ = tx;
    }

    void ViewAnimatorParams::setTranslateY(double ty) {
        translate_y_ = ty;
    }

    void ViewAnimatorParams::setSPivotX(double px) {
        s_pivot_x_ = px;
    }

    void ViewAnimatorParams::setSPivotY(double py) {
        s_pivot_y_ = py;
    }

    void ViewAnimatorParams::setRPivotX(double px) {
        r_pivot_x_ = px;
    }

    void ViewAnimatorParams::setRPivotY(double py) {
        r_pivot_y_ = py;
    }

    void ViewAnimatorParams::setHasReveal(bool reveal) {
        has_reveal_ = reveal;
    }

    double ViewAnimatorParams::getAlpha() const {
        return alpha_;
    }

    double ViewAnimatorParams::getScaleX() const {
        return scale_x_;
    }

    double ViewAnimatorParams::getScaleY() const {
        return scale_y_;
    }

    double ViewAnimatorParams::getRotate() const {
        return rotate_;
    }

    double ViewAnimatorParams::getTranslateX() const {
        return translate_x_;
    }

    double ViewAnimatorParams::getTranslateY() const {
        return translate_y_;
    }

    double ViewAnimatorParams::getSPivotX() const {
        return s_pivot_x_;
    }

    double ViewAnimatorParams::getSPivotY() const {
        return s_pivot_y_;
    }

    double ViewAnimatorParams::getRPivotX() const {
        return r_pivot_x_;
    }

    double ViewAnimatorParams::getRPivotY() const {
        return r_pivot_y_;
    }

    bool ViewAnimatorParams::hasReveal() const {
        return has_reveal_;
    }

    ViewRevealTVals& ViewAnimatorParams::reveal() {
        return reveal_tvals_;
    }

    void ViewAnimatorParams::setOrder(Order order) {
        order_ = order;
    }

    ViewAnimatorParams::Order ViewAnimatorParams::getOrder() const {
        return order_;
    }

    void ViewAnimatorParams::generateMatrix(int x, int y, Matrix2x3F* out) const {
        out->identity();

        switch (order_) {
        case TSR:
            out->preRotate(
                float(rotate_), float(r_pivot_x_ + x), float(r_pivot_y_ + y));
            out->preScale(
                float(scale_x_), float(scale_y_), float(s_pivot_x_ + x), float(s_pivot_y_ + y));
            out->preTranslate(float(translate_x_), float(translate_y_));
            break;

        case TRS:
            out->preScale(
                float(scale_x_), float(scale_y_), float(s_pivot_x_ + x), float(s_pivot_y_ + y));
            out->preRotate(
                float(rotate_), float(r_pivot_x_ + x), float(r_pivot_y_ + y));
            out->preTranslate(float(translate_x_), float(translate_y_));
            break;

        case STR:
            out->preRotate(
                float(rotate_), float(r_pivot_x_ + x), float(r_pivot_y_ + y));
            out->preTranslate(float(translate_x_), float(translate_y_));
            out->preScale(
                float(scale_x_), float(scale_y_), float(s_pivot_x_ + x), float(s_pivot_y_ + y));
            break;

        case SRT:
            out->preTranslate(float(translate_x_), float(translate_y_));
            out->preRotate(
                float(rotate_), float(r_pivot_x_ + x), float(r_pivot_y_ + y));
            out->preScale(
                float(scale_x_), float(scale_y_), float(s_pivot_x_ + x), float(s_pivot_y_ + y));
            break;

        case RTS:
            out->preScale(
                float(scale_x_), float(scale_y_), float(s_pivot_x_ + x), float(s_pivot_y_ + y));
            out->preTranslate(float(translate_x_), float(translate_y_));
            out->preRotate(
                float(rotate_), float(r_pivot_x_ + x), float(r_pivot_y_ + y));
            break;

        case RST:
            out->preTranslate(float(translate_x_), float(translate_y_));
            out->preScale(
                float(scale_x_), float(scale_y_), float(s_pivot_x_ + x), float(s_pivot_y_ + y));
            out->preRotate(
                float(rotate_), float(r_pivot_x_ + x), float(r_pivot_y_ + y));
            break;
        default:
            break;
        }

    }

}
