// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_VIEW_ANIMATOR_H_
#define UKIVE_ANIMATION_VIEW_ANIMATOR_H_

#include "ukive/animation/animation_director.h"


namespace ukive {

    class View;

    class ViewAnimator : public AnimationDirectorListener {
    public:
        enum RevealShape {
            REVEAL_RECT = 0,
            REVEAL_CIRCULE = 1,
        };

        using FinishedHandler = std::function<void(AnimationDirector*)>;

        explicit ViewAnimator(View* v);
        ~ViewAnimator();

        void start();
        void cancel();
        ViewAnimator* setDuration(uint64_t duration);

        ViewAnimator* alpha(double value);
        ViewAnimator* scaleX(double value);
        ViewAnimator* scaleY(double value);
        ViewAnimator* rotate(double value);
        ViewAnimator* translateX(double value);
        ViewAnimator* translateY(double value);
        ViewAnimator* rectReveal(
            double center_x, double center_y,
            double start_hori_radius, double end_hori_radius,
            double start_vert_radius, double end_vert_radius);
        ViewAnimator* circleReveal(
            double center_x, double center_y, double start_radius, double end_radius);

        ViewAnimator* setListener(AnimationDirectorListener* l);
        ViewAnimator* setFinishedHandler(const FinishedHandler& h);

        void onPreViewDraw();
        void onPostViewDraw();

        // AnimationDirectorListener
        void onDirectorStarted(AnimationDirector* director, const Animator* animator) override;
        void onDirectorProgress(AnimationDirector* director, const Animator* animator) override;
        void onDirectorStopped(AnimationDirector* director, const Animator* animator) override;
        void onDirectorFinished(AnimationDirector* director, const Animator* animator) override;
        void onDirectorReset(AnimationDirector* director, const Animator* animator) override;

    private:
        enum ViewAnimId {
            VIEW_ANIM_ALPHA = 1,
            VIEW_ANIM_SCALE_X,
            VIEW_ANIM_SCALE_Y,
            VIEW_ANIM_TRANSLATE_X,
            VIEW_ANIM_TRANSLATE_Y,
            VIEW_ANIM_ROTATE,
            VIEW_ANIM_RECT_REVEAL_R_X,
            VIEW_ANIM_RECT_REVEAL_R_Y,
            VIEW_ANIM_CIRCLE_REVEAL_R,
        };

        uint64_t duration_;
        View* owner_view_;
        AnimationDirector director_;

        FinishedHandler finished_handler_;
        AnimationDirectorListener* listener_;
    };

}

#endif  // UKIVE_ANIMATION_VIEW_ANIMATOR_H_