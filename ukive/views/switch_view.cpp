// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "switch_view.h"

#include "ukive/animation/interpolator.h"
#include "ukive/event/input_consts.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/views/check_listener.h"


namespace {

    const ukive::Color kNormalTrackColor = ukive::Color::Grey400;
    const ukive::Color kNormalThumbColor = ukive::Color::Grey200;
    const ukive::Color kCheckedTrackColor = ukive::Color::Blue200;
    const ukive::Color kCheckedThumbColor = ukive::Color::Blue500;

}

namespace ukive {

    SwitchView::SwitchView(Context c)
        : SwitchView(c, {}) {}

    SwitchView::SwitchView(Context c, AttrsRef attrs)
        : super(c, attrs)
    {
        using namespace std::chrono_literals;
        anim_.setDuration(200ms);
        anim_.setInterpolator(new LinearInterpolator());
        anim_.setListener(this);
        anim_.setValueRange(0, 1);

        def_track_width_ = c.dp2pxi(34);
        def_track_height_ = c.dp2pxi(14);
        def_thumb_radius_ = int(def_track_height_ / 2.f * 1.6f);

        setClickable(true);
        setFocusable(true);
    }

    SwitchView::~SwitchView() {
    }

    void SwitchView::setChecked(bool checked) {
        if (checked_ == checked) {
            return;
        }

        if (listener_) {
            listener_->onCheckChanged(this, checked);
        }

        checked_ = checked;
        anim_.reset();
        auto range = def_track_width_ - def_track_height_;
        if (checked_) {
            anim_.setInitValue(cur_pos_ / double(range));
        } else {
            anim_.setInitValue(1 - cur_pos_ / double(range));
        }
        anim_.start();
        startVSync();
        requestDraw();
    }

    void SwitchView::setOnCheckListener(OnCheckListener* l) {
        listener_ = l;
    }

    Size SwitchView::onDetermineSize(const SizeInfo& info) {
        int final_w = def_track_width_ + (std::max)(def_thumb_radius_ * 2 - def_track_height_, 0);
        int final_h = (std::max)(def_track_height_, def_thumb_radius_ * 2);
        final_w += getPadding().hori();
        final_h += getPadding().vert();

        return getPreferredSize(info, final_w, final_h);
    }

    void SwitchView::onDraw(Canvas* canvas) {
        super::onDraw(canvas);

        auto range = def_track_width_ - def_track_height_;
        float cur_value = cur_pos_ / float(range);

        // 底部的滑动槽
        {
            Color target_color;
            interpolateColor(
                kNormalTrackColor, kCheckedTrackColor,
                cur_value, &target_color);

            int x = (std::max)(def_thumb_radius_ - def_track_height_ / 2, 0);
            canvas->fillRoundRect(
                RectF(float(x), float(x), float(def_track_width_), float(def_track_height_)),
                float(def_track_height_ / 2), target_color);
        }

        // 滑块
        {
            int cx = (std::max)(def_thumb_radius_, def_track_height_ / 2);
            float target_cx = float(cx + cur_pos_);
            Color target_color;
            interpolateColor(
                kNormalThumbColor, kCheckedThumbColor, cur_value, &target_color);

            canvas->fillCircle({ target_cx, float(cx) }, float(cx), target_color);
        }
    }

    bool SwitchView::onInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                start_x_ = e->getX();
                start_pos_ = cur_pos_;
                anim_.stop();
            }
            break;

        case InputEvent::EVT_DOWN:
            start_x_ = e->getX();
            start_pos_ = cur_pos_;
            anim_.stop();
            break;

        case InputEvent::EVM_MOVE:
        case InputEvent::EVT_MOVE:
            if (isPressed()) {
                auto distance = e->getX() - start_x_;
                int r = getContext().dp2pxi(
                    (e->getEvent() == InputEvent::EVT_MOVE) ?
                    kTouchScrollingThreshold : kMouseScrollingThreshold);
                if (!is_moved_ && std::abs(distance) > r) {
                    is_moved_ = true;
                    start_x_ = e->getX();
                    distance = 0;
                }

                if (is_moved_) {
                    auto range = def_track_width_ - def_track_height_;
                    auto cur = start_pos_ + distance;
                    cur_pos_ = (std::min)((std::max)(0, cur), range);
                    requestDraw();
                }
            }
            break;

        case InputEvent::EVM_UP:
            if (e->getMouseKey() == InputEvent::MK_LEFT &&
                isPressed())
            {
                doUpAction();
                is_moved_ = false;
            }
            break;

        case InputEvent::EVT_UP:
            if (isLocalPointerInThisVisible(e) &&
                isPressed())
            {
                doUpAction();
                is_moved_ = false;
            }
            break;

        case InputEvent::EV_LEAVE_VIEW:
            is_moved_ = false;
            break;

        default:
            break;
        }

        return super::onInputEvent(e);
    }

    void SwitchView::doUpAction() {
        auto range = def_track_width_ - def_track_height_;
        if (checked_) {
            if (cur_pos_ >= range / 2 && is_moved_) {
                anim_.reset();
                anim_.setInitValue(cur_pos_ / double(range));
                anim_.start();
                startVSync();
                requestDraw();
            } else {
                setChecked(false);
            }
        } else {
            if (cur_pos_ < range / 2 && is_moved_) {
                anim_.reset();
                anim_.setInitValue(1 - cur_pos_ / double(range));
                anim_.start();
                startVSync();
                requestDraw();
            } else {
                setChecked(true);
            }
        }
    }

    void SwitchView::interpolateColor(
        const Color& start, const Color& end, float value, Color* out) const
    {
        out->r = start.r + (end.r - start.r) * value;
        out->g = start.g + (end.g - start.g) * value;
        out->b = start.b + (end.b - start.b) * value;
        out->a = start.a + (end.a - start.a) * value;
    }

    void SwitchView::onAnimationProgress(Animator* animator) {
        auto range = def_track_width_ - def_track_height_;
        if (checked_) {
            cur_pos_ = int(animator->getCurValue() * range);
        } else {
            cur_pos_ = int((1 - animator->getCurValue()) * range);
        }
    }

    void SwitchView::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!anim_.update(start_time, display_freq)) {
            stopVSync();
        }
        requestDraw();
    }

}