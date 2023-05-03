// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "text_blink.h"

#include <cmath>

#include "ukive/graphics/canvas.h"
#include "ukive/views/view.h"


namespace ukive {

    TextBlink::TextBlink(View* v)
        : target_view_(v),
          is_located_(false),
          blink_mask_(false),
          color_(Color::Black)
    {
        thickness_ = v->getContext().dp2px(2);

        using namespace std::chrono_literals;

        timer_.setDuration(500ms);
        timer_.setRepeat(true);
        timer_.setRunner(std::bind(&TextBlink::onRun, this));
    }

    TextBlink::~TextBlink() {
    }

    void TextBlink::draw(Canvas* canvas) {
        if (blink_mask_ && is_located_) {
            canvas->fillRect(RectF(blink_rect_), color_);
        }
    }

    void TextBlink::show() {
        if (!is_located_) {
            return;
        }

        blink_mask_ = false;
        onRun();

        timer_.start();
    }

    void TextBlink::hide() {
        timer_.stop();

        blink_mask_ = false;

        Rect bounds(blink_rect_);
        bounds.offset(
            target_view_->getPadding().start() - target_view_->getScrollX(),
            target_view_->getPadding().top() - target_view_->getScrollY());
        target_view_->requestDraw(bounds);
    }

    void TextBlink::locate(float x_center, float top, float bottom) {
        Rect new_rect;
        calculateRect(x_center, top, bottom, &new_rect);
        locate(new_rect);
    }

    void TextBlink::locate(const Rect& rect) {
        auto new_rect = rect;

        // 防止光标被截断
        if (new_rect.x() < 0) {
            new_rect.offset(-new_rect.x(), 0);
        }
        if (new_rect.right() > target_view_->getContentBounds().width() + target_view_->getScrollX()) {
            new_rect.offset(target_view_->getContentBounds().width() + target_view_->getScrollX() - new_rect.right(), 0);
        }

        if (blink_rect_ == new_rect) {
            return;
        }

        bool should_show = false;
        if (timer_.isRunning()) {
            hide();
            should_show = true;
        }

        blink_rect_ = new_rect;

        is_located_ = true;

        if (should_show) {
            show();
        }
    }

    void TextBlink::calculateRect(
        float x_center, float top, float bottom, Rect* out) const
    {
        out->xywh(
            int(std::round(x_center - thickness_ / 2.f)),
            int(std::floor(top)),
            int(std::round(thickness_)),
            int(std::ceil(bottom - top)));
    }

    void TextBlink::setColor(const Color& color) {
        color_ = color;
    }

    void TextBlink::setThickness(float thickness) {
        thickness_ = thickness;
    }

    bool TextBlink::isShowing() const {
        return timer_.isRunning();
    }

    const Color& TextBlink::getColor() const {
        return color_;
    }

    float TextBlink::getThickness() const {
        return thickness_;
    }

    void TextBlink::onRun() {
        blink_mask_ = !blink_mask_;

        // blink_rect_ 采用文字布局坐标，原点位于 View 的绘制起始点，
        // 和 View 的原点相差一个 padding
        Rect bounds(blink_rect_);
        bounds.offset(
            target_view_->getPadding().start() - target_view_->getScrollX(),
            target_view_->getPadding().top() - target_view_->getScrollY());
        target_view_->requestDraw(bounds);
    }

}
