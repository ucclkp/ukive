// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "element.h"

#include "ukive/graphics/canvas.h"
#include "ukive/window/context.h"


namespace ukive {

    Element::Element()
        : shape_(SHAPE_RECT),
          has_solid_(false) {}

    Element::Element(const Color& color)
        : Element(SHAPE_RECT, color) {}

    Element::Element(Shape shape, const Color& color)
        : shape_(shape),
          has_solid_(true),
          solid_color_(color) {}

    void Element::setBounds(const Rect &rect) {
        if (bounds_ == rect) {
            return;
        }

        bounds_ = rect;
        onBoundChanged(bounds_);
    }

    void Element::setBounds(int x, int y, int width, int height) {
        if (bounds_.x() == x
            && bounds_.y() == y
            && bounds_.width() == width
            && bounds_.height() == height)
        {
            return;
        }

        bounds_.xywh(x, y, width, height);

        onBoundChanged(bounds_);
    }

    void Element::setCallback(ElementCallback* callback) {
        callback_ = callback;
    }

    bool Element::setState(int state) {
        if (state == state_) {
            return false;
        }

        prev_state_ = state_;
        state_ = state;

        return onStateChanged(state, prev_state_);
    }

    void Element::setHotspot(int x, int y) {
        start_pos_ = {x, y};
    }

    bool Element::setParentFocus(bool focus) {
        if (focus == is_parent_has_focus_) {
            return false;
        }

        is_parent_has_focus_ = focus;
        return onFocusChanged(is_parent_has_focus_);
    }

    bool Element::resetState() {
        if (prev_state_ == STATE_NONE &&
            state_ == STATE_NONE)
        {
            return false;
        }
        prev_state_ = STATE_NONE;
        state_ = STATE_NONE;

        return onStateReset();
    }

    void Element::notifyContextChanged(Context::Type type, const Context& context) {
        onContextChanged(type, context);
    }

    int Element::getState() const {
        return state_;
    }

    int Element::getPrevState() const {
        return prev_state_;
    }

    Rect Element::getBounds() const {
        return bounds_;
    }

    ElementCallback* Element::getCallback() const {
        return callback_;
    }

    bool Element::isParentHasFocus() const {
        return is_parent_has_focus_;
    }

    void Element::draw(Canvas* canvas) {
        Rect bound = getBounds();

        switch (shape_) {
        case SHAPE_RECT: {
            if (has_solid_) {
                canvas->fillRect(RectF(bound), solid_color_);
            }
            if (has_stroke_) {
                canvas->drawRect(RectF(bound), stroke_width_, stroke_color_);
            }
            break;
        }
        case SHAPE_RRECT: {
            if (has_solid_) {
                canvas->fillRoundRect(RectF(bound), round_radius_, solid_color_);
            }
            if (has_stroke_) {
                canvas->drawRoundRect(RectF(bound), stroke_width_, round_radius_, stroke_color_);
            }
            break;
        }
        case SHAPE_OVAL: {
            float cx = bound.width() / 2.f;
            float cy = bound.height() / 2.f;

            if (has_solid_) {
                canvas->fillOval(PointF{ cx, cy }, cx, cy, solid_color_);
            }
            if (has_stroke_) {
                canvas->drawOval(PointF{ cx, cy }, cx, cy, stroke_width_, stroke_color_);
            }
            break;
        }
        }

        for (auto& e : list_) {
            e->draw(canvas);
        }
    }

    bool Element::isTransparent() const {
        if (has_solid_) {
            return solid_color_.a == 0.0f;
        }

        if (has_stroke_) {
            return false;
        }

        for (auto& e : list_) {
            if (!e->isTransparent()) {
                return false;
            }
        }

        return true;
    }

    void Element::requestDraw() {
        if (callback_) {
            callback_->onElementRequestDraw(this);
        }
    }

    bool Element::onFocusChanged(bool focus) {
        return false;
    }

    void Element::onBoundChanged(const Rect& new_bound) {
        for (auto& e : list_) {
            e->setBounds(new_bound);
        }
    }

    bool Element::onStateChanged(int new_state, int prev_state) {
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

        for (auto& e : list_) {
            need_redraw |= e->setState(new_state);
        }

        return need_redraw;
    }

    bool Element::onStateReset() {
        bool need_redraw = false;

        for (auto& e : list_) {
            need_redraw |= e->onStateReset();
        }
        return need_redraw;
    }

    void Element::onContextChanged(
        Context::Type type, const Context& context)
    {
        for (auto& e : list_) {
            e->onContextChanged(type, context);
        }
    }

    int Element::getContentWidth() const {
        return -1;
    }

    int Element::getContentHeight() const {
        return -1;
    }

    void Element::setRadius(float radius) {
        if (radius != round_radius_) {
            round_radius_ = radius;
            requestDraw();
        }
    }

    void Element::setShape(Shape shape) {
        if (shape != shape_) {
            shape_ = shape;
            requestDraw();
        }
    }

    void Element::setSolidEnable(bool enable) {
        has_solid_ = enable;
    }

    void Element::setSolidColor(const Color& c) {
        solid_color_ = c;
        requestDraw();
    }

    void Element::setStrokeEnable(bool enable) {
        has_stroke_ = enable;
    }

    void Element::setStrokeWidth(float width) {
        stroke_width_ = width;
    }

    void Element::setStrokeColor(const Color& color) {
        stroke_color_ = color;
    }

    void Element::add(Element* element) {
        if (!element) {
            return;
        }
        list_.push_back(std::shared_ptr<Element>(element));
    }

    void Element::remove(Element* element) {
        if (!element) {
            return;
        }

        for (auto it = list_.begin();
            it != list_.end();)
        {
            if ((*it).get() == element) {
                it = list_.erase(it);
            } else {
                ++it;
            }
        }
    }

    Element::Shape Element::getShape() const {
        return shape_;
    }

    const Color& Element::getSolidColor() const {
        return solid_color_;
    }

    bool Element::hasSolid() const {
        return has_solid_;
    }

    bool Element::hasStroke() const {
        return has_stroke_;
    }

}