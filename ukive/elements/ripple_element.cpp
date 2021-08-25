// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ripple_element.h"

#include <cmath>

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/window/window.h"
#include "ukive/animation/interpolator.h"

#define DOWN_UP_SEC      200ms
#define HOVER_LEAVE_SEC  200ms

#define HOVER_ALPHA  0.07
#define DOWN_ALPHA   0.13


namespace ukive {

    RippleElement::RippleElement()
        : MultiElement(),
          alpha_(0)
    {
        using namespace std::chrono_literals;

        ripple_animator_.setDuration(500ms);
        ripple_animator_.setInterpolator(new LinearInterpolator());
        ripple_animator_.setFinalValue(1);

        hover_animator_.setListener(this);
        hover_animator_.setDuration(HOVER_LEAVE_SEC);
        hover_animator_.setInterpolator(new LinearInterpolator());
        hover_animator_.setFinalValue(HOVER_ALPHA);

        down_animator_.setListener(this);
        down_animator_.setDuration(DOWN_UP_SEC);
        down_animator_.setInterpolator(new LinearInterpolator());
        down_animator_.setFinalValue(DOWN_ALPHA);

        up_animator_.setListener(this);
        up_animator_.setDuration(DOWN_UP_SEC);

        leave_animator_.setListener(this);
        leave_animator_.setDuration(HOVER_LEAVE_SEC);
        leave_animator_.setInterpolator(new LinearInterpolator());
        leave_animator_.setFinalValue(0);
    }

    RippleElement::~RippleElement() {
    }

    void RippleElement::setTintColor(Color tint) {
        tint_color_ = tint;
    }

    void RippleElement::setDrawMaskEnabled(bool enabled) {
        if (is_draw_mask_ != enabled) {
            is_draw_mask_ = enabled;
            requestDraw();
        }
    }

    void RippleElement::draw(Canvas *canvas) {
        auto bound = getBounds();
        Color color(0.f, 0.f, 0.f, float(alpha_));

        bool has_content = tint_color_.a > 0.f || color.a > 0.f || ripple_animator_.isRunning();
        if (has_content) {
            // 绘制底色、alpha 和 ripple。
            if (!content_off_ ||
                content_off_->getWidth() != bound.width() ||
                content_off_->getHeight() != bound.height() ||
                content_off_->getBuffer()->getImageOptions() != canvas->getBuffer()->getImageOptions())
            {
                content_off_ = std::make_unique<Canvas>(
                    bound.width(), bound.height(),
                    canvas->getBuffer()->getImageOptions());
            }

            content_off_->beginDraw();
            content_off_->clear();
            content_off_->setOpacity(canvas->getOpacity());
            if (tint_color_.a > 0.f) {
                content_off_->fillRect(RectF(bound), tint_color_);
            }
            if (color.a > 0.f) {
                content_off_->fillRect(RectF(bound), color);
            }

            if (ripple_animator_.isRunning()) {
                auto w = bound.width();
                auto h = bound.height();
                auto r = std::sqrt(w * w + h * h);

                Color ripple_color = Color::ofRGB(0, float(1 - ripple_animator_.getCurValue()) * 0.1f);
                content_off_->fillCircle(
                    PointF(Point(start_x_, start_y_)),
                    float(ripple_animator_.getCurValue() * r), ripple_color);
            }
            content_off_->endDraw();
            std::shared_ptr<ImageFrame> content_img(content_off_->extractImage());

            if (list_.empty()) {
                canvas->drawImage(content_img.get());
            } else {
                // 绘制 mask，以该 mask 确定背景形状以及 ripple 的扩散边界。
                if (!mask_off_ ||
                    mask_off_->getWidth() != bound.width() ||
                    mask_off_->getHeight() != bound.height() ||
                    mask_off_->getBuffer()->getImageOptions() != canvas->getBuffer()->getImageOptions())
                {
                    mask_off_ = std::make_unique<Canvas>(
                        bound.width(), bound.height(),
                        canvas->getBuffer()->getImageOptions());
                }
                mask_off_->beginDraw();
                mask_off_->clear();
                mask_off_->setOpacity(canvas->getOpacity());
                MultiElement::draw(mask_off_.get());
                mask_off_->endDraw();
                std::shared_ptr<ImageFrame> mask_img(mask_off_->extractImage());

                if (is_draw_mask_) {
                    canvas->drawImage(mask_img.get());
                }
                canvas->fillOpacityMask(
                    float(bound.width()), float(bound.height()),
                    mask_img.get(), content_img.get());
            }
        } else {
            if (is_draw_mask_) {
                MultiElement::draw(canvas);
            }
        }
    }

    bool RippleElement::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = MultiElement::onStateChanged(new_state, prev_state);

        switch (new_state) {
        case STATE_NONE:
        {
            if (prev_state == STATE_HOVERED) {
                if (hover_animator_.isRunning()) {
                    hover_animator_.stop();
                    alpha_ = hover_animator_.getCurValue();
                } else if (up_animator_.isRunning()) {
                    up_animator_.stop();
                    alpha_ = up_animator_.getCurValue();
                }

                leave_animator_.setInitValue(alpha_);
                leave_animator_.reset();
                leave_animator_.start();
                startVSync();

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_.stop();
                alpha_ = down_animator_.getCurValue();

                up_animator_.setValueRange(alpha_, 0);
                up_animator_.setInterpolator(new LinearInterpolator());
                up_animator_.reset();
                up_animator_.start();
                startVSync();

                need_redraw |= true;
            }
            break;
        }

        case STATE_HOVERED:
            if (prev_state == STATE_NONE) {
                alpha_ = 0;
                hover_animator_.reset();
                hover_animator_.start();
                startVSync();

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_.stop();
                alpha_ = down_animator_.getCurValue();

                up_animator_.setValueRange(alpha_, HOVER_ALPHA);
                up_animator_.setInterpolator(new LinearInterpolator());
                up_animator_.reset();
                up_animator_.start();

                ripple_animator_.reset();
                ripple_animator_.start();
                startVSync();

                need_redraw |= true;
            }
            break;

        case STATE_PRESSED:
            hover_animator_.stop();
            alpha_ = hover_animator_.getCurValue();

            down_animator_.setInitValue(alpha_);
            down_animator_.reset();
            down_animator_.start();
            startVSync();

            need_redraw |= true;
            break;

        case STATE_DISABLED:
            up_animator_.stop();
            down_animator_.stop();
            hover_animator_.stop();
            leave_animator_.stop();
            ripple_animator_.stop();
            alpha_ = 0;

            need_redraw |= true;
            break;

        default:
            break;
        }

        return need_redraw;
    }

    bool RippleElement::onStateReset() {
        up_animator_.stop();
        down_animator_.stop();
        hover_animator_.stop();
        leave_animator_.stop();
        ripple_animator_.stop();
        alpha_ = 0;
        return true;
    }

    void RippleElement::onContextChanged(const Context& context) {
        switch (context.getChanged()) {
        case Context::DEV_LOST:
        {
            mask_off_.reset();
            content_off_.reset();
            break;
        }

        case Context::DEV_RESTORE:
        {
            requestDraw();
            break;
        }

        default:
            break;
        }
    }

    Element::Opacity RippleElement::getOpacity() const {
        return OPA_OPAQUE;
    }

    void RippleElement::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        hover_animator_.update(start_time, display_freq);
        leave_animator_.update(start_time, display_freq);
        down_animator_.update(start_time, display_freq);
        up_animator_.update(start_time, display_freq);
        ripple_animator_.update(start_time, display_freq);

        if (hover_animator_.isRunning() ||
            leave_animator_.isRunning() ||
            down_animator_.isRunning() ||
            up_animator_.isRunning() ||
            ripple_animator_.isRunning())
        {
            requestVSync();
        } else {
            stopVSync();
        }
        requestDraw();
    }

    void RippleElement::onAnimationProgress(Animator* animator) {
        alpha_ = animator->getCurValue();
    }

}
