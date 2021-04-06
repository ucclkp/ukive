// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shape_element.h"

#include "ukive/graphics/canvas.h"


namespace ukive {

    ShapeElement::ShapeElement(Shape shape)
        : Element(),
          width_(-1),
          height_(-1),
          shape_(shape),
          has_solid_(false),
          has_stroke_(false),
          round_radius_(1.f),
          stroke_width_(1.f) {
    }

    ShapeElement::~ShapeElement() {
    }


    void ShapeElement::setSize(int width, int height) {
        width_ = width;
        height_ = height;
    }

    void ShapeElement::setRadius(float radius) {
        round_radius_ = radius;
    }

    void ShapeElement::setShape(Shape shape) {
        if (shape == shape_) {
            return;
        }

        shape_ = shape;
        requestDraw();
    }

    void ShapeElement::setSolidEnable(bool enable) {
        has_solid_ = enable;
    }

    void ShapeElement::setSolidColor(Color color) {
        solid_color_ = color;
    }

    void ShapeElement::setStrokeEnable(bool enable) {
        has_stroke_ = enable;
    }

    void ShapeElement::setStrokeWidth(float width) {
        stroke_width_ = width;
    }

    void ShapeElement::setStrokeColor(Color color) {
        stroke_color_ = color;
    }

    void ShapeElement::draw(Canvas* canvas) {
        if (!has_solid_ && !has_stroke_) {
            return;
        }

        Rect bound;
        if (width_ == -1 || height_ == -1) {
            bound = getBounds();
        } else {
            bound = Rect(0, 0, width_, height_);
        }

        switch (shape_) {
        case RECT: {
            if (has_solid_) {
                canvas->fillRect(RectF(bound), solid_color_);
            }
            if (has_stroke_) {
                canvas->drawRect(RectF(bound), stroke_width_, stroke_color_);
            }
            break;
        }
        case ROUND_RECT: {
            if (has_solid_) {
                canvas->fillRoundRect(RectF(bound), round_radius_, solid_color_);
            }
            if (has_stroke_) {
                canvas->drawRoundRect(RectF(bound), stroke_width_, round_radius_, stroke_color_);
            }
            break;
        }
        case OVAL: {
            float cx = (bound.right - bound.left) / 2.f;
            float cy = (bound.bottom - bound.top) / 2.f;

            if (has_solid_) {
                canvas->fillOval(PointF(cx, cy), cx, cy, solid_color_);
            }
            if (has_stroke_) {
                canvas->drawOval(PointF(cx, cy), cx, cy, stroke_width_, stroke_color_);
            }
            break;
        }
        }
    }

    int ShapeElement::getShape() const {
        return shape_;
    }

    Element::Opacity ShapeElement::getOpacity() const {
        if ((has_solid_ && solid_color_.a != 0.f)
            || (has_stroke_ && stroke_color_.a != 0.f && stroke_width_ != 0.f)) {
            return OPA_OPAQUE;
        } else {
            return OPA_TRANSPARENT;
        }
    }

    int ShapeElement::getContentWidth() const {
        return width_;
    }

    int ShapeElement::getContentHeight() const {
        return height_;
    }

    bool ShapeElement::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = false;
        if (new_state == STATE_DISABLED) {
            if (has_solid_) {
                solid_color_.r *= 0.9f;
                solid_color_.g *= 0.9f;
                solid_color_.b *= 0.9f;
                need_redraw = true;
            }
            if (has_stroke_) {
                stroke_color_.r *= 0.9f;
                stroke_color_.g *= 0.9f;
                stroke_color_.b *= 0.9f;
                need_redraw = true;
            }
        } else if (prev_state == STATE_DISABLED) {
            if (has_solid_) {
                solid_color_.r /= 0.9f;
                solid_color_.g /= 0.9f;
                solid_color_.b /= 0.9f;
                need_redraw = true;
            }
            if (has_stroke_) {
                stroke_color_.r /= 0.9f;
                stroke_color_.g /= 0.9f;
                stroke_color_.b /= 0.9f;
                need_redraw = true;
            }
        }

        return need_redraw;
    }

}