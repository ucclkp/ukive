// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_VIEW_ANIMATOR_H_
#define UKIVE_ANIMATION_VIEW_ANIMATOR_H_

#include <functional>

#include "ukive/animation/animation_director.h"
#include "ukive/graphics/vsyncable.h"
#include "ukive/resources/tval.hpp"


namespace ukive {

    class View;

    class ViewAnimator : public AnimationDirectorListener, public VSyncable {
    public:
        using ns = utl::TimeUtils::ns;
        using nsp = utl::TimeUtils::nsp;

        enum RevealShape {
            REVEAL_RECT = 0,
            REVEAL_CIRCULE = 1,
        };

        using FinishedHandler = std::function<void(AnimationDirector*)>;

        explicit ViewAnimator(View* v);
        ~ViewAnimator();

        void start();
        void cancel();

        ViewAnimator& alpha(double value, nsp duration);
        ViewAnimator& scaleX(double value, nsp duration);
        ViewAnimator& scaleY(double value, nsp duration);
        ViewAnimator& rotate(double value, nsp duration);
        ViewAnimator& translateX(double value, nsp duration);
        ViewAnimator& translateY(double value, nsp duration);
        ViewAnimator& rectReveal(
            tvalcr x, tvalcr y,
            tvalcr start_width, tvalcr end_width,
            tvalcr start_height, tvalcr end_height, nsp duration);
        ViewAnimator& circleReveal(
            tvalcr center_x, tvalcr center_y,
            tvalcr start_radius, tvalcr end_radius, nsp duration);

        ViewAnimator& setListener(AnimationDirectorListener* l);
        ViewAnimator& setFinishedHandler(const FinishedHandler& h);

        void onPreViewDraw();
        void onPostViewDraw();

    protected:
        // VSyncable
        void onVSync(
            uint64_t start_time, uint32_t display_freq, uint32_t real_interval) override;

        // AnimationDirectorListener
        void onDirectorStarted(AnimationDirector* director) override;
        void onDirectorProgress(AnimationDirector* director) override;
        void onDirectorStopped(AnimationDirector* director) override;
        void onDirectorFinished(AnimationDirector* director) override;
        void onDirectorReset(AnimationDirector* director) override;
        void onDirectorAnitomStarted(
            AnimationDirector* director, const Anitom* anitom) override;
        void onDirectorAnitomProgress(
            AnimationDirector* director, const Anitom* anitom) override;
        void onDirectorAnitomStopped(
            AnimationDirector* director, const Anitom* anitom) override;
        void onDirectorAnitomFinished(
            AnimationDirector* director, const Anitom* anitom) override;
        void onDirectorAnitomReset(
            AnimationDirector* director, const Anitom* anitom) override;

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

        View* owner_view_;
        AnimationDirector director_;

        FinishedHandler finished_handler_;
        AnimationDirectorListener* listener_;
    };

}

#endif  // UKIVE_ANIMATION_VIEW_ANIMATOR_H_