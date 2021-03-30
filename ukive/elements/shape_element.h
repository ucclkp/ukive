// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_SHAPE_ELEMENT_H_
#define UKIVE_ELEMENTS_SHAPE_ELEMENT_H_

#include "ukive/elements/element.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class ShapeElement : public Element {
    public:
        enum Shape {
            RECT,
            ROUND_RECT,
            OVAL
        };

        explicit ShapeElement(Shape shape);
        ~ShapeElement();

        void setSize(int width, int height);
        void setRadius(float radius);

        void setShape(Shape shape);
        void setSolidEnable(bool enable);
        void setSolidColor(Color color);
        void setStrokeEnable(bool enable);
        void setStrokeWidth(float width);
        void setStrokeColor(Color color);

        void draw(Canvas* canvas) override;

        int getShape() const;
        Opacity getOpacity() const override;
        int getContentWidth() const override;
        int getContentHeight() const override;

    protected:
        bool onStateChanged(int new_state, int prev_state) override;

    private:
        int width_;
        int height_;
        Shape shape_;

        bool has_solid_;
        bool has_stroke_;
        float round_radius_;
        float stroke_width_;
        Color solid_color_;
        Color stroke_color_;
    };

}

#endif  // UKIVE_ELEMENTS_SHAPE_ELEMENT_H_