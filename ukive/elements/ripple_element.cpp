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
#include "ukive/window/window.h"
#include "ukive/animation/interpolator.h"

#define DOWN_UP_SEC      200ms
#define HOVER_LEAVE_SEC  200ms


namespace ukive {

    RippleElement::RippleElement()
        : alpha_(0)
    {
        initElement();
    }

    RippleElement::RippleElement(const Color& color, bool draw_mask)
        : RippleElement(SHAPE_RECT, color, draw_mask) {}

    RippleElement::RippleElement(Shape shape, const Color& color, bool draw_mask)
        : super(shape, color),
          alpha_(0),
          is_draw_mask_(draw_mask)
    {
        initElement();
    }

    RippleElement::~RippleElement() {
    }

    void RippleElement::initElement() {
        down_alpha_ = 0.13;
        hover_alpha_ = 0.07;
        interact_color_ = Color::Transparent;

        using namespace std::chrono_literals;

        ripple_animator_.setDuration(500ms);
        ripple_animator_.setInterpolator(new LinearInterpolator());
        ripple_animator_.setFinalValue(1);

        hover_animator_.setListener(this);
        hover_animator_.setDuration(HOVER_LEAVE_SEC);
        hover_animator_.setInterpolator(new LinearInterpolator());
        hover_animator_.setFinalValue(hover_alpha_);

        down_animator_.setListener(this);
        down_animator_.setDuration(DOWN_UP_SEC);
        down_animator_.setInterpolator(new LinearInterpolator());
        down_animator_.setFinalValue(down_alpha_);

        up_animator_.setListener(this);
        up_animator_.setDuration(DOWN_UP_SEC);

        leave_animator_.setListener(this);
        leave_animator_.setDuration(HOVER_LEAVE_SEC);
        leave_animator_.setInterpolator(new LinearInterpolator());
        leave_animator_.setFinalValue(0);
    }

    void RippleElement::setDownAlpha(double a) {
        down_alpha_ = a;
        down_animator_.setFinalValue(down_alpha_);
    }

    void RippleElement::setHoverAlpha(double a) {
        hover_alpha_ = a;
        hover_animator_.setFinalValue(hover_alpha_);
    }

    void RippleElement::setInteractColor(const Color& c) {
        interact_color_ = c;
        requestDraw();
    }

    void RippleElement::setDrawMaskEnabled(bool enabled) {
        if (is_draw_mask_ != enabled) {
            is_draw_mask_ = enabled;
            requestDraw();
        }
    }

    void RippleElement::draw(Canvas *canvas) {
        auto bound = getBounds();
        Color color(interact_color_, float(alpha_));

        bool has_content = color.a > 0.f || ripple_animator_.isRunning();
        if (has_content) {
            // 绘制底色、alpha 和 ripple。
            if (!content_off_ ||
                content_off_->getSize() != bound.size() ||
                content_off_->getImageOptions() != canvas->getImageOptions())
            {
                content_off_ = std::make_unique<Canvas>(
                    bound.width(), bound.height(),
                    canvas->getImageOptions());
            }

            content_off_->beginDraw();
            content_off_->clear();
            content_off_->setOpacity(canvas->getOpacity());
            if (color.a > 0.f) {
                content_off_->fillRect(RectF(bound), color);
            }

            if (ripple_animator_.isRunning()) {
                auto w = bound.width();
                auto h = bound.height();
                auto r = std::sqrt(w * w + h * h);

                Color ripple_color = Color::ofRGB(0, float(1 - ripple_animator_.getCurValue()) * 0.1f);
                content_off_->fillCircle(
                    PointF(start_pos_),
                    float(ripple_animator_.getCurValue() * r), ripple_color);
            }
            content_off_->endDraw();
            auto content_img = content_off_->extractImage();

            if (!hasSolid()) {
                if (is_draw_mask_) {
                    super::draw(canvas);
                }
                canvas->drawImage(content_img.get());
            } else {
                // 绘制 mask，以该 mask 确定背景形状以及 ripple 的扩散边界。
                if (!mask_off_ ||
                    mask_off_->getSize() != bound.size() ||
                    mask_off_->getImageOptions() != canvas->getImageOptions())
                {
                    mask_off_ = std::make_unique<Canvas>(
                        bound.width(), bound.height(),
                        canvas->getImageOptions());
                }
                mask_off_->beginDraw();
                mask_off_->clear();
                mask_off_->setOpacity(canvas->getOpacity());
                super::draw(mask_off_.get());
                mask_off_->endDraw();
                auto mask_img = mask_off_->extractImage();

                if (is_draw_mask_) {
                    canvas->drawImage(mask_img.get());
                }
                canvas->fillOpacityMask(
                    float(bound.width()), float(bound.height()),
                    mask_img.get(), content_img.get());
            }
        } else {
            if (is_draw_mask_) {
                super::draw(canvas);
            }
        }
    }

    bool RippleElement::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = super::onStateChanged(new_state, prev_state);

        switch (new_state) {
        case STATE_NONE:
        {
            if (prev_state == STATE_HOVERED) {
                hover_animator_.stop();
                up_animator_.stop();
                leave_animator_.setInitValue(alpha_);
                leave_animator_.reset();
                leave_animator_.start();
                startVSync();

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_.stop();
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
                up_animator_.stop();
                hover_animator_.setInitValue(alpha_);
                hover_animator_.reset();
                hover_animator_.start();
                startVSync();

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_.stop();
                up_animator_.setValueRange(alpha_, hover_alpha_);
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
        super::onStateReset();

        up_animator_.stop();
        down_animator_.stop();
        hover_animator_.stop();
        leave_animator_.stop();
        ripple_animator_.stop();
        alpha_ = 0;
        return true;
    }

    bool RippleElement::isTransparent() const {
        bool has_content = alpha_ > 0.f || ripple_animator_.isRunning();
        if (has_content) {
            return false;
        }

        if (is_draw_mask_) {
            return super::isTransparent();
        }

        return true;
    }

    void RippleElement::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        bool ret = hover_animator_.update(start_time, display_freq);
        ret |= leave_animator_.update(start_time, display_freq);
        ret |= down_animator_.update(start_time, display_freq);
        ret |= up_animator_.update(start_time, display_freq);
        ret |= ripple_animator_.update(start_time, display_freq);

        if (!ret) {
            stopVSync();
        }
        requestDraw();
    }

    void RippleElement::onAnimationProgress(Animator* animator) {
        alpha_ = animator->getCurValue();
    }

}
