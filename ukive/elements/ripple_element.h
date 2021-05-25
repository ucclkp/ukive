// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_RIPPLE_ELEMENT_H_
#define UKIVE_ELEMENTS_RIPPLE_ELEMENT_H_

#include "ukive/animation/animator.h"
#include "ukive/elements/multi_element.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/vsyncable.h"


namespace ukive {

    class RippleElement :
        public MultiElement,
        public AnimationListener,
        public VSyncable
    {
    public:
        RippleElement();
        ~RippleElement();

        void setTintColor(Color tint);
        void setDrawMaskEnabled(bool enabled);

        void draw(Canvas* canvas) override;

        Opacity getOpacity() const override;

        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

        // AnimationListener
        void onAnimationProgress(Animator* animator) override;

    protected:
        bool onStateChanged(int new_state, int prev_state) override;
        bool onStateReset() override;
        void onContextChanged(const Context& context) override;

    private:
        double alpha_;
        Color tint_color_;
        bool is_draw_mask_ = true;

        Animator up_animator_;
        Animator down_animator_;
        Animator hover_animator_;
        Animator leave_animator_;
        Animator ripple_animator_;

        std::unique_ptr<Canvas> mask_off_;
        std::unique_ptr<Canvas> content_off_;
    };

}

#endif  // UKIVE_ELEMENTS_RIPPLE_ELEMENT_H_