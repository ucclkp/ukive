// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_SWITCH_VIEW_H_
#define UKIVE_VIEWS_SWITCH_VIEW_H_

#include "ukive/animation/animator.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/views/view.h"


namespace ukive {

    class OnCheckListener;

    class SwitchView :
        public View,
        public AnimationListener,
        public VSyncable
    {
    public:
        explicit SwitchView(Context c);
        SwitchView(Context c, AttrsRef attrs);
        ~SwitchView();

        void setChecked(bool checked);
        void setOnCheckListener(OnCheckListener* l);

        // View
        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        // AnimationListener
        void onAnimationProgress(Animator* animator) override;

        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

    private:
        using super = View;

        void doUpAction();
        void interpolateColor(
            const Color& start, const Color& end, float value, Color* out) const;

        bool checked_ = false;
        int def_track_width_;
        int def_track_height_;
        int def_thumb_radius_;
        Animator anim_;
        OnCheckListener* listener_ = nullptr;

        int start_x_ = 0;
        int cur_pos_ = 0;
        int start_pos_ = 0;
        bool is_moved_ = false;
    };

}

#endif  // UKIVE_VIEWS_SWITCH_VIEW_H_