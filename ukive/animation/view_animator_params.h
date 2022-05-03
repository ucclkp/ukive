// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_VIEW_ANIMATOR_PARAMS_H_
#define UKIVE_ANIMATION_VIEW_ANIMATOR_PARAMS_H_

#include "ukive/animation/view_tval.h"
#include "ukive/graphics/matrix_2x3.hpp"


namespace ukive {

    class ViewAnimatorParams {
    public:
        enum Order {
            TSR,
            TRS,
            STR,
            SRT,
            RTS,
            RST,
        };

        ViewAnimatorParams();

        void setAlpha(double alpha);
        void setScaleX(double sx);
        void setScaleY(double sy);
        void setRotate(double rotate);
        void setTranslateX(double tx);
        void setTranslateY(double ty);
        void setSPivotX(double px);
        void setSPivotY(double py);
        void setRPivotX(double px);
        void setRPivotY(double py);
        void setHasReveal(bool reveal);

        double getAlpha() const;
        double getScaleX() const;
        double getScaleY() const;
        double getRotate() const;
        double getTranslateX() const;
        double getTranslateY() const;
        double getSPivotX() const;
        double getSPivotY() const;
        double getRPivotX() const;
        double getRPivotY() const;

        bool hasReveal() const;
        ViewRevealTVals& reveal();

        void setOrder(Order order);
        Order getOrder() const;
        void generateMatrix(int x, int y, Matrix2x3F* out) const;

    private:
        // 普通动画变量。
        double alpha_ = 1.0;
        double scale_x_ = 1.0;
        double scale_y_ = 1.0;
        double rotate_ = 0.0;
        double translate_x_ = 0.0;
        double translate_y_ = 0.0;
        double s_pivot_x_ = 0.0, s_pivot_y_ = 0.0;
        double r_pivot_x_ = 0.0, r_pivot_y_ = 0.0;

        // 揭露动画变量。
        bool has_reveal_ = false;
        ViewRevealTVals reveal_tvals_;

        Order order_ = TSR;
    };

}

#endif  // UKIVE_ANIMATION_VIEW_ANIMATOR_PARAMS_H_