// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "seek_bar.h"

#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"
#include "ukive/animation/interpolator.h"


namespace ukive {

    SeekBar::SeekBar(Context c)
        : SeekBar(c, {}) {}

    SeekBar::SeekBar(Context c, AttrsRef attrs)
        : super(c, attrs)
    {
        initSeekBar();
    }

    SeekBar::~SeekBar() {
    }

    void SeekBar::initSeekBar()
    {
        maximum_ = 100.f;
        seek_percent_ = 0.f;
        listener_ = nullptr;

        track_height_ = getContext().dp2pxi(2);
        if (track_height_ % 2 != 0) {
            ++track_height_;
        }

        thumb_min_diameter_ = getContext().dp2pxi(10);
        if (thumb_min_diameter_ % 2 != 0) {
            ++thumb_min_diameter_;
        }

        thumb_max_diameter_ = getContext().dp2pxi(14);
        if (thumb_max_diameter_ % 2 != 0) {
            ++thumb_max_diameter_;
        }

        using namespace std::chrono_literals;
        thumb_animator_.setDuration(200ms);
        thumb_animator_.setInterpolator(new LinearInterpolator());
        thumb_animator_.setValueRange(0, 1);

        setFocusable(true);
        setTouchCapturable(true);
    }

    void SeekBar::setMaximum(float maximum) {
        if (maximum_ > 0) {
            maximum_ = maximum;
        }
    }

    void SeekBar::setProgress(float progress, bool notify) {
        float prog = (std::min)(maximum_, progress);
        float percent = prog / maximum_;
        if (percent != seek_percent_) {
            seek_percent_ = percent;

            if (notify && listener_) {
                listener_->onSeekValueChanged(this, seek_percent_*maximum_);
                listener_->onSeekIntegerValueChanged(this, static_cast<int>(seek_percent_*maximum_));
            }

            requestDraw();
        }
    }

    float SeekBar::getProgress() {
        return maximum_ * seek_percent_;
    }

    void SeekBar::setOnSeekValueChangedListener(OnSeekValueChangedListener* l) {
        listener_ = l;
    }

    bool SeekBar::isPointerInThumb(int x, int y) {
        float thumb_radius = thumb_max_diameter_ / 2.f;
        int track_width = getWidth() - thumb_max_diameter_ - getPadding().hori();

        float thumb_center_x_in_track = track_width * seek_percent_ + thumb_radius;
        float thumb_center_y_in_track = getPadding().top() + thumb_radius;

        if (((x - getPadding().start()) >= (thumb_center_x_in_track - thumb_radius))
            && ((x - getPadding().start()) <= (thumb_center_x_in_track + thumb_radius)))
        {
            if ((y - getPadding().top()) >= (thumb_center_y_in_track - thumb_radius)
                && (y - getPadding().top()) <= (thumb_center_y_in_track + thumb_radius))
                return true;
        }

        return false;
    }

    bool SeekBar::isPointerInTrack(int x, int y) {
        int trackSpace = getWidth() - getPadding().hori();

        if ((x - getPadding().start()) >= 0 &&
            (x - getPadding().start()) <= trackSpace)
        {
            if ((y - getPadding().top()) >= 0 &&
                (y - getPadding().top()) <= thumb_max_diameter_)
            {
                return true;
            }
        }

        return false;
    }

    void SeekBar::computePercent(int x, int y) {
        float mouse_in_track = x - getPadding().start() - thumb_max_diameter_ / 2.f;
        int track_width = getWidth() - thumb_max_diameter_ - getPadding().hori();
        seek_percent_ = (std::max)(0.f, mouse_in_track / track_width);
        seek_percent_ = (std::min)(1.f, seek_percent_);

        if (listener_) {
            listener_->onSeekValueChanged(this, seek_percent_*maximum_);
            listener_->onSeekIntegerValueChanged(this, static_cast<int>(seek_percent_*maximum_));
        }
    }

    void SeekBar::startZoomInAnimation() {
        if (is_on_thumb_) {
            return;
        }
        is_on_thumb_ = true;

        if (thumb_animator_.isFinished()) {
            thumb_animator_.reset();
        }

        thumb_animator_.start();
        startVSync();
        requestDraw();
    }

    void SeekBar::startZoomOutAnimation() {
        if (!is_on_thumb_) {
            return;
        }
        is_on_thumb_ = false;

        if (thumb_animator_.isFinished()) {
            thumb_animator_.reset();
        }

        thumb_animator_.start();
        startVSync();
        requestDraw();
    }

    Size SeekBar::onDetermineSize(const SizeInfo& info) {
        int final_width = 0;
        int final_height = 0;

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
            final_width = info.width().val;
            break;

        case SizeInfo::FREEDOM:
            final_width = 0;
            break;

        case SizeInfo::DEFINED:
        default:
            final_width = info.width().val;
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            final_height = (std::min)(info.height().val, thumb_max_diameter_ + getPadding().vert());
            break;

        case SizeInfo::FREEDOM:
            final_height = thumb_max_diameter_ + getPadding().vert();
            break;

        case SizeInfo::DEFINED:
        default:
            final_height = info.height().val;
            break;
        }

        return Size(final_width, final_height);
    }

    void SeekBar::onDraw(Canvas* canvas) {
        super::onDraw(canvas);

        float left = thumb_max_diameter_ / 2.f;
        float top = (thumb_max_diameter_ - track_height_) / 2.f;
        int trackWidth = getContentBounds().width() - thumb_max_diameter_;

        float cur_pos = trackWidth * seek_percent_;
        float center_x = left + cur_pos;
        float center_y = thumb_max_diameter_ / 2.f;

        // 进度条
        if (center_x < thumb_min_diameter_) {
            canvas->fillRect(
                RectF(left, top, float(trackWidth), float(track_height_)), Color::Grey300);
        } else {
            canvas->fillRect(
                RectF(left, top, cur_pos, float(track_height_)), Color::Blue400);
            canvas->fillRect(
                RectF(center_x, top, trackWidth - cur_pos, float(track_height_)), Color::Grey300);
        }

        float thumb_cur_diameter;
        if (is_on_thumb_) {
            if (thumb_animator_.isFinished() || !thumb_animator_.isStarted()) {
                thumb_cur_diameter = float(thumb_max_diameter_);
            } else {
                thumb_cur_diameter = thumb_min_diameter_
                    + float((thumb_max_diameter_ - thumb_min_diameter_) * thumb_animator_.getCurValue());
            }
        } else {
            if (thumb_animator_.isFinished() || !thumb_animator_.isStarted()) {
                thumb_cur_diameter = float(thumb_min_diameter_);
            } else {
                thumb_cur_diameter = thumb_min_diameter_
                    + float((thumb_max_diameter_ - thumb_min_diameter_) * (1 - thumb_animator_.getCurValue()));
            }
        }
        canvas->fillCircle(
            { center_x, center_y }, thumb_cur_diameter / 2.f, Color::Blue400);
    }

    bool SeekBar::onInputEvent(InputEvent* e) {
        bool result = super::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                if (isPointerInThumb(e->getX(), e->getY())) {
                    result = true;
                    computePercent(e->getX(), e->getY());
                    requestDraw();
                } else if (isPointerInTrack(e->getX(), e->getY())) {
                    result = true;
                    computePercent(e->getX(), e->getY());
                    startZoomInAnimation();
                }
            }
            break;
        }

        case InputEvent::EVT_DOWN:
        {
            if (isPointerInThumb(e->getX(), e->getY()) ||
                isPointerInTrack(e->getX(), e->getY()))
            {
                result = true;
                start_touch_x_ = e->getX();
                start_touch_y_ = e->getY();
                capture_touch_ = false;
            }
            break;
        }

        case InputEvent::EVM_MOVE:
        {
            if (isPressed()) {
                computePercent(e->getX(), e->getY());
                requestDraw();
            } else {
                if (isPointerInThumb(e->getX(), e->getY())) {
                    startZoomInAnimation();
                } else {
                    startZoomOutAnimation();
                }
            }
            result = true;
            break;
        }

        case InputEvent::EVT_MOVE:
        {
            if (isPressed()) {
                if (!capture_touch_) {
                    int distance = e->getX() - start_touch_x_;
                    if (std::abs(distance) > getContext().dp2pxi(8)) {
                        capture_touch_ = true;
                        startZoomInAnimation();
                    }
                }

                if (capture_touch_) {
                    result = true;
                    computePercent(e->getX(), e->getY());
                    requestDraw();
                }
            }
            break;
        }

        case InputEvent::EVM_UP:
        {
            result = true;
            break;
        }

        case InputEvent::EVT_UP:
        {
            result = true;
            startZoomOutAnimation();
            break;
        }

        case InputEvent::EVM_SCROLL_ENTER:
        {
            result = true;
            break;
        }

        case InputEvent::EV_LEAVE_VIEW:
        {
            startZoomOutAnimation();
            break;
        }

        default:
            break;
        }

        return result;
    }

    void SeekBar::onVSync(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        if (!thumb_animator_.update(start_time, display_freq)) {
            stopVSync();
        }
        requestDraw();
    }

}
