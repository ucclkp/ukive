// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_ANIMATOR_H_
#define UKIVE_ANIMATION_ANIMATOR_H_

#include <memory>

#include "ukive/animation/timer.h"


namespace ukive {

    class Animator;
    class Interpolator;

    class AnimationListener {
    public:
        virtual ~AnimationListener() = default;

        virtual void onAnimationStarted(Animator* animator) {}
        virtual void onAnimationProgress(Animator* animator) {}
        virtual void onAnimationStopped(Animator* animator) {}
        virtual void onAnimationFinished(Animator* animator) {}
        virtual void onAnimationReset(Animator* animator) {}
    };

    class Animator {
    public:
        explicit Animator(bool timer_driven = false);
        ~Animator();

        void start();
        void stop();
        void finish();
        void reset();
        void update();

        void setId(int id);
        void setFps(int fps);
        void setRepeat(bool repeat);
        void setDuration(uint64_t duration);
        void setInterpolator(Interpolator* ipr);
        void setListener(AnimationListener* listener);
        void setInitValue(double init_val);

        bool isRepeat() const;
        bool isStarted() const;
        bool isRunning() const;
        bool isFinished() const;

        int getId() const;
        int getFps() const;
        uint64_t getDuration() const;
        double getCurValue() const;
        double getInitValue() const;
        Interpolator* getInterpolator() const;

    private:
        static uint64_t upTimeMillis();

        void restart();
        void AnimationProgress();

        int id_;
        int fps_;
        double cur_val_;
        double init_val_;
        uint64_t duration_;
        uint64_t elapsed_duration_;
        uint64_t start_time_;

        bool is_repeat_;
        bool is_started_;
        bool is_running_;
        bool is_finished_;
        bool is_timer_driven_;

        Timer timer_;
        AnimationListener* listener_;
        std::unique_ptr<Interpolator> interpolator_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATOR_H_