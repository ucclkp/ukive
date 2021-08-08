// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_VIEW_TVAL_H_
#define UKIVE_ANIMATION_VIEW_TVAL_H_

#include "ukive/graphics/rect.hpp"
#include "ukive/resources/tval.hpp"


namespace ukive {

    class View;

    class ViewTVal {
    public:
        ViewTVal();
        ViewTVal(tvalcr s, tvalcr e);

        static double resolve(View* v, tvalcr tv);

        void set(tvalcr tv);
        void set(tvalcr s, tvalcr e);
        void reset();

        const tval& getStartTVal() const { return start_val_; }
        const tval& getEndTVal() const   { return end_val_; }

        double getStart(View* v) const;
        double getEnd(View* v) const;
        double get(View* v, double interpolation) const;

        void calculate(View* v, double interpolation);
        double getRV(View* v) const;

    private:
        tval start_val_;
        tval end_val_;
        bool is_evaluated_ = false;
        double real_val_ = 0;
    };

    class ViewRevealTVals {
    public:
        enum class Type {
            None,
            Circle,
            Rect,
        };

        ViewRevealTVals();

        void setCircle(
            tvalcr center_x, tvalcr center_y,
            tvalcr start_radius, tvalcr end_radius);
        void setRect(
            tvalcr x, tvalcr y,
            tvalcr start_width, tvalcr end_width,
            tvalcr start_height, tvalcr end_height);
        void reset();

        Type getType() const { return type_; }

        RectD getStart(View* v) const;
        RectD getEnd(View* v) const;
        RectD get(View* v, double interpolation) const;

        void calculate(View* v, double interpolation);
        RectD getRV(View* v) const;

    private:
        static double resolveX(View* v, tvalcr x_tv, double width);
        static double resolveY(View* v, tvalcr y_tv, double height);
        static double resolveRadius(View* v, tvalcr r_tv);
        static double resolveWidth(View* v, tvalcr w_tv);
        static double resolveHeight(View* v, tvalcr h_tv);

        Type type_;
        ViewTVal x_;
        ViewTVal y_;
        ViewTVal width_;
        ViewTVal height_;

        bool is_evaluated_ = false;
        RectD real_val_;
    };

}

#endif  // UKIVE_ANIMATION_VIEW_TVAL_H_