// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/radio_button.h"

#include <cmath>

#include "ukive/animation/interpolator.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/views/radio_selected_listener.h"
#include "ukive/window/window.h"


namespace ukive {

    // static
    int RadioButton::g_group_id_ = 0;
    std::map<int, std::vector<RadioButton*>> RadioButton::g_groups_;

    // static
    void RadioButton::StartGroup() {
    }

    // static
    void RadioButton::EndGroup() {
        ++g_group_id_;
    }

    RadioButton::RadioButton(Context c)
        : RadioButton(c, {}) {}

    RadioButton::RadioButton(Context c, AttrsRef attrs)
        : super(c, attrs),
          selected_(false)
    {
        group_id_ = g_group_id_;
        g_groups_[g_group_id_].push_back(this);

        using namespace std::chrono_literals;
        anim_.setDuration(200ms);
        anim_.setInterpolator(new LinearInterpolator());
        anim_.setValueRange(0, 1);

        Rect space;
        space.left = c.dp2pxi(24);
        setSpace(space);

        setClickable(true);
        setFocusable(true);
    }

    RadioButton::~RadioButton() {
        auto vec = g_groups_.find(group_id_);
        if (vec != g_groups_.end()) {
            for (auto it = vec->second.begin();
                it != vec->second.end(); ++it)
            {
                if ((*it) == this) {
                    vec->second.erase(it);
                    break;
                }
            }
        }
    }

    void RadioButton::setChecked(bool checked) {
        if (checked == selected_) {
            return;
        }
        selected_ = checked;

        if (listener_) {
            listener_->onRadioSelected(this);
        }

        if (selected_) {
            auto vec = g_groups_.find(group_id_);
            if (vec != g_groups_.end()) {
                for (auto it = vec->second.begin();
                    it != vec->second.end(); ++it)
                {
                    if ((*it) != this) {
                        (*it)->setChecked(false);
                    }
                }
            }
        }

        if (selected_) {
            anim_.reset();
            anim_.start();
            startVSync();
        } else {
            anim_.stop();
        }
        requestDraw();
    }

    void RadioButton::setOnSelectedListener(OnRadioSelectedListener* l) {
        listener_ = l;
    }

    void RadioButton::onDraw(Canvas* canvas) {
        super::onDraw(canvas);

        auto bounds = getContentBounds();
        int length = (std::min)(getContext().dp2pxi(20), bounds.height());

        int stroke_width = getContext().dp2pxi(2.f);

        float radius = (length - stroke_width * 2) / 2.f;
        float cx = stroke_width + radius;
        float cy = (bounds.height() - length) / 2.f + stroke_width + radius;
        canvas->drawCircle(PointF(cx, cy), radius, float(stroke_width), Color::Blue400);

        if (selected_) {
            auto value = static_cast<float>(anim_.getCurValue());
            auto cur_radius = value * radius * 0.6f;
            canvas->fillCircle(PointF(cx, cy), cur_radius, Color::Blue600);
        }
    }

    bool RadioButton::onInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_UP:
        case InputEvent::EVT_UP:
            if (isLocalPointerInThisVisible(e) &&
                e->getMouseKey() == InputEvent::MK_LEFT &&
                isPressed() && !selected_)
            {
                setChecked(true);
            }
            break;

        default:
            break;
        }

        return super::onInputEvent(e);
    }

    void RadioButton::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!anim_.update(start_time, display_freq)) {
            stopVSync();
        }
        requestDraw();
    }

}
