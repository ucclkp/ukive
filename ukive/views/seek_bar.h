// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_SEEK_BAR_H_
#define UKIVE_VIEWS_SEEK_BAR_H_

#include "ukive/views/view.h"
#include "ukive/animation/animator.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {

    class SeekBar : public View, public VSyncable {
    public:
        explicit SeekBar(Context c);
        SeekBar(Context c, AttrsRef attrs);
        ~SeekBar();

        void setMaximum(float maximum);
        void setProgress(float progress, bool notify = false);
        float getProgress();

        void setOnSeekValueChangedListener(OnSeekValueChangedListener* l);

        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

    private:
        using super = View;

        void initSeekBar();

        bool isPointerInThumb(int x, int y);
        bool isPointerInTrack(int x, int y);
        void computePercent(int x, int y);

        void startZoomInAnimation();
        void startZoomOutAnimation();

        int track_height_;
        int thumb_min_diameter_;
        int thumb_max_diameter_;

        float maximum_;
        float seek_percent_;
        bool is_on_thumb_ = false;

        int start_touch_x_ = 0;
        int start_touch_y_ = 0;
        bool capture_touch_ = false;

        Animator thumb_animator_;
        OnSeekValueChangedListener* listener_;
    };

}

#endif  // UKIVE_VIEWS_SEEK_BAR_H_