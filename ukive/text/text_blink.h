// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_BLINK_H_
#define UKIVE_TEXT_TEXT_BLINK_H_

#include "ukive/animation/timer.h"
#include "ukive/graphics/rect.hpp"
#include "ukive/graphics/colors/color.h"


namespace ukive {

    class View;
    class Canvas;
    class Cycler;

    class TextBlink {
    public:
        explicit TextBlink(View* v);
        ~TextBlink();

        void draw(Canvas* canvas);

        void show();
        void hide();
        void locate(float x_center, float top, float bottom);
        void locate(const Rect& rect);
        void calculateRect(float x_center, float top, float bottom, Rect* out) const;

        void setColor(const Color& color);
        void setThickness(float thickness);

        bool isShowing() const;
        const Color& getColor() const;
        float getThickness() const;

        void onRun();

    private:
        View* target_view_;

        bool is_located_;
        bool blink_mask_;

        Rect blink_rect_;

        float thickness_;
        Color color_;
        Timer timer_;
    };

}

#endif  // UKIVE_TEXT_TEXT_BLINK_H_