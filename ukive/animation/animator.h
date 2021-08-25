// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_ANIMATOR_H_
#define UKIVE_ANIMATION_ANIMATOR_H_

#include <memory>

#include "utils/time_utils.h"


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
        using ns = utl::TimeUtils::ns;
        using nsp = utl::TimeUtils::nsp;

        static uint64_t now();

        Animator();
        ~Animator();

        void start();
        void stop();
        void finish();
        void reset();
        bool update(uint64_t cur_time, uint32_t display_freq);

        void setId(int id);

        void setRepeat(bool repeat);
        void setDuration(nsp duration);
        void setInterpolator(Interpolator* ipr);
        void setListener(AnimationListener* listener);

        void setInitValue(double init_val);
        void setFinalValue(double final_val);
        void setValueRange(double init_val, double final_val);

        bool isRepeat() const;
        bool isStarted() const;
        bool isRunning() const;
        bool isFinished() const;

        int getId() const;
        ns getDuration() const;
        double getCurValue() const;
        double getInitValue() const;
        double getFinalValue() const;
        Interpolator* getInterpolator() const;

    private:
        void restart(uint64_t cur_time);
        friend class AnimationDirector;

        int id_;
        double cur_val_;
        double init_val_ = 0;
        double final_val_ = 1;

        uint64_t duration_;
        uint64_t elapsed_duration_;
        uint64_t start_time_;

        bool is_repeat_;
        bool is_preparing_ = false;
        bool is_started_;
        bool is_running_;
        bool is_finished_;

        AnimationListener* listener_;
        std::unique_ptr<Interpolator> interpolator_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATOR_H_