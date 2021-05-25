// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_
#define UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_

#include <map>
#include <memory>

#include "ukive/animation/animator.h"


namespace ukive {

    class Animator;
    class AnimationDirector;

    class AnimationDirectorListener {
    public:
        virtual ~AnimationDirectorListener() = default;

        virtual void onDirectorStarted(AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorProgress(AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorStopped(AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorFinished(AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorReset(AnimationDirector* director, const Animator* animator) {}
    };

    class AnimationDirector : public AnimationListener {
    public:
        AnimationDirector();

        void addAnimator(int id);
        void removeAnimator(int id);
        bool hasAnimator(int id) const;
        const Animator* getAnimator(int id) const;

        void start();
        void stop();
        void finish();
        void reset();
        void update(uint64_t cur_time, uint32_t display_freq);

        void setDuration(int id, Animator::nsp duration);
        void setInitValue(int id, double init_val);
        void setInterpolator(int id, Interpolator* ipr);
        void setListener(AnimationDirectorListener* l);

        bool isRunning() const;
        bool isFinished() const;

        Animator::ns getDuration(int id) const;
        double getCurValue(int id) const;
        double getInitValue(int id) const;
        Interpolator* getInterpolator(int id) const;

    private:
        // AnimationListener
        void onAnimationStarted(Animator* animator) override;
        void onAnimationProgress(Animator* animator) override;
        void onAnimationStopped(Animator* animator) override;
        void onAnimationFinished(Animator* animator) override;
        void onAnimationReset(Animator* animator) override;

        AnimationDirectorListener* listener_;
        std::map<int, std::unique_ptr<Animator>> animators_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_