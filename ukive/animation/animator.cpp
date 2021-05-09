// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/animator.h"

#include "ukive/animation/interpolator.h"
#include "ukive/system/time_utils.h"


namespace ukive {

    Animator::Animator(bool timer_driven)
        : id_(0),
          fps_(120),
          cur_val_(0),
          init_val_(0),
          duration_(250),
          elapsed_duration_(0),
          start_time_(0),
          is_repeat_(false),
          is_started_(false),
          is_running_(false),
          is_finished_(false),
          is_timer_driven_(timer_driven),
          listener_(nullptr),
          interpolator_(std::make_unique<LinearInterpolator>(1))
    {
        if (is_timer_driven_) {
            timer_.setRepeat(true);
            timer_.setRunner(std::bind(&Animator::AnimationProgress, this));
            timer_.setDuration(1000 / fps_);
        }
    }

    Animator::~Animator() {}

    void Animator::start() {
        if (is_running_ || is_finished_) {
            return;
        }

        if (!is_started_) {
            is_started_ = true;
            cur_val_ = init_val_;
            interpolator_->setInitVal(init_val_);
        }

        is_finished_ = false;
        start_time_ = upTimeMillis() - elapsed_duration_;

        if (is_timer_driven_) {
            timer_.start();
        }
        is_running_ = true;

        if (listener_) {
            listener_->onAnimationStarted(this);
        }
    }

    void Animator::stop() {
        if (is_running_) {
            if (is_timer_driven_) {
                timer_.stop();
            }
            is_running_ = false;

            elapsed_duration_ = upTimeMillis() - start_time_;

            if (listener_) {
                listener_->onAnimationStopped(this);
            }
        }
    }

    void Animator::finish() {
        if (is_started_) {
            if (is_timer_driven_) {
                timer_.stop();
            }
            is_running_ = false;
            is_finished_ = true;

            cur_val_ = interpolator_->interpolate(1);

            if (listener_) {
                listener_->onAnimationFinished(this);
            }
        }
    }

    void Animator::reset() {
        if (is_started_) {
            if (is_timer_driven_) {
                timer_.stop();
            }
            is_started_ = false;
            is_running_ = false;
            is_finished_ = false;

            cur_val_ = init_val_;
            elapsed_duration_ = 0;

            if (listener_) {
                listener_->onAnimationReset(this);
            }
        }
    }

    void Animator::update() {
        if (!is_running_) {
            return;
        }

        auto cur_time = upTimeMillis();
        bool finished = (cur_time >= start_time_ + duration_);
        double progress = finished ? 1 : static_cast<double>(cur_time - start_time_) / duration_;
        cur_val_ = interpolator_->interpolate(progress);

        if (listener_) {
            listener_->onAnimationProgress(this);
        }

        if (finished) {
            if (is_repeat_) {
                restart();
            } else {
                finish();
            }
        }
    }

    void Animator::restart() {
        cur_val_ = init_val_;
        elapsed_duration_ = 0;
        start_time_ = upTimeMillis();
    }

    void Animator::setId(int id) {
        id_ = id;
    }

    void Animator::setFps(int fps) {
        if (fps <= 0) {
            return;
        }

        fps_ = fps;
        if (is_timer_driven_) {
            timer_.setDuration(1000 / fps_);
        }
    }

    void Animator::setRepeat(bool repeat) {
        is_repeat_ = repeat;
    }

    void Animator::setDuration(uint64_t duration) {
        duration_ = duration;
    }

    void Animator::setInterpolator(Interpolator* ipr) {
        if (!ipr || interpolator_.get() == ipr) {
            return;
        }
        interpolator_.reset(ipr);
    }

    void Animator::setListener(AnimationListener* listener) {
        listener_ = listener;
    }

    void Animator::setInitValue(double init_val) {
        init_val_ = init_val;
    }

    bool Animator::isRepeat() const {
        return is_repeat_;
    }

    bool Animator::isStarted() const {
        return is_started_;
    }

    bool Animator::isRunning() const {
        return is_running_;
    }

    bool Animator::isFinished() const {
        return is_finished_;
    }

    int Animator::getId() const {
        return id_;
    }

    int Animator::getFps() const {
        return fps_;
    }

    uint64_t Animator::getDuration() const {
        return duration_;
    }

    double Animator::getCurValue() const {
        return is_started_ ? cur_val_ : init_val_;
    }

    double Animator::getInitValue() const {
        return init_val_;
    }

    Interpolator* Animator::getInterpolator() const {
        return interpolator_.get();
    }

    void Animator::AnimationProgress() {
        update();
    }

    // static
    uint64_t Animator::upTimeMillis() {
        return TimeUtils::upTimeMillis();
    }

}
