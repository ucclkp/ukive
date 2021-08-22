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

        virtual void onDirectorStarted(AnimationDirector* director) {}
        virtual void onDirectorProgress(AnimationDirector* director) {}
        virtual void onDirectorStopped(AnimationDirector* director) {}
        virtual void onDirectorFinished(AnimationDirector* director) {}
        virtual void onDirectorReset(AnimationDirector* director) {}

        virtual void onDirectorAnimationStarted(
            AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorAnimationProgress(
            AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorAnimationStopped(
            AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorAnimationFinished(
            AnimationDirector* director, const Animator* animator) {}
        virtual void onDirectorAnimationReset(
            AnimationDirector* director, const Animator* animator) {}
    };

    class AnimationDirector : public AnimationListener {
    public:
        using ns = Animator::ns;
        using nsp = Animator::nsp;
        using APtr = std::unique_ptr<Animator>;

        struct Animation {
            ns st;
            APtr ptr;
        };

        using APtrVec = std::vector<Animation>;

        AnimationDirector();

        Animator* add(int id);
        Animator* add(int id, nsp st);
        void remove(int id);
        void remove(int id, nsp st);
        void clear();
        bool contains(int id) const;
        bool contains(int id, nsp st) const;
        const APtrVec& get(int id) const;
        Animator* get(int id, nsp st) const;

        void start();
        void stop();
        void finish();
        void reset();
        void update(uint64_t cur_time, uint32_t display_freq);

        void setListener(AnimationDirectorListener* l);

        bool isRunning() const;
        bool isFinished() const;

        double getCurValue(int id) const;
        double getInitValue(int id) const;
        ns getDuration(int id) const;
        ns getTotalDuration() const;

    private:
        void restart(uint64_t cur_time);

        // AnimationListener
        void onAnimationStarted(Animator* animator) override;
        void onAnimationProgress(Animator* animator) override;
        void onAnimationStopped(Animator* animator) override;
        void onAnimationFinished(Animator* animator) override;
        void onAnimationReset(Animator* animator) override;

        uint64_t start_time_ = 0;
        uint64_t elapsed_time_ = 0;

        bool is_repeat_;
        bool is_started_;
        bool is_running_;
        bool is_finished_;
        AnimationDirectorListener* listener_;
        std::map<int, APtrVec> animators_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_