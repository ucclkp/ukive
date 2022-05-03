// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/animator.h"

#include "ukive/animation/interpolator.h"


namespace {

    template <typename Ty>
    Ty uns_sub(Ty lhs, Ty rhs) {
        return (lhs >= rhs) ? (lhs - rhs) : 0;
    }

}

namespace ukive {

    Animator::Animator()
        : id_(0),
          cur_val_(0),
          duration_(250),
          elapsed_duration_(0),
          start_time_(0),
          is_repeat_(false),
          is_started_(false),
          is_running_(false),
          is_finished_(false),
          listener_(nullptr),
          interpolator_(std::make_unique<LinearInterpolator>())
    {}

    Animator::~Animator() {
        stop();
    }

    void Animator::start() {
        if (is_running_ || is_finished_) {
            return;
        }

        if (!is_started_) {
            is_started_ = true;
            cur_val_ = init_val_;
        }

        is_preparing_ = true;
        is_running_ = true;
        is_finished_ = false;
        start_time_ = 0;

        if (listener_) {
            listener_->onAnimationStarted(this);
        }
    }

    void Animator::stop() {
        if (!is_running_) {
            return;
        }

        is_running_ = false;

        if (!is_preparing_) {
            elapsed_duration_ = uns_sub(now(), start_time_);
        }

        if (listener_) {
            listener_->onAnimationStopped(this);
        }
    }

    void Animator::finish() {
        if (!is_started_ || is_finished_) {
            return;
        }

        is_preparing_ = false;
        is_running_ = false;
        is_finished_ = true;

        cur_val_ = final_val_;

        if (listener_) {
            listener_->onAnimationFinished(this);
        }
    }

    void Animator::reset() {
        if (!is_started_) {
            return;
        }

        is_preparing_ = false;
        is_started_ = false;
        is_running_ = false;
        is_finished_ = false;

        cur_val_ = init_val_;
        elapsed_duration_ = 0;
        start_time_ = 0;

        if (listener_) {
            listener_->onAnimationReset(this);
        }
    }

    bool Animator::update(uint64_t cur_time, uint32_t display_freq) {
        if (!is_running_) {
            return false;
        }

        if (is_preparing_) {
            is_preparing_ = false;
            start_time_ = uns_sub(cur_time, (std::nano::den / display_freq) + elapsed_duration_);
        }

        bool finished = cur_time >= start_time_ + duration_;
        double progress = finished ? 1.0 : double(uns_sub(cur_time, start_time_)) / duration_;
        cur_val_ = interpolator_->interpolate(init_val_, final_val_, progress);

        if (listener_) {
            listener_->onAnimationProgress(this);
        }

        if (finished) {
            if (is_repeat_) {
                restart(cur_time);
            } else {
                finish();
            }
        }

        return true;
    }

    void Animator::restart(uint64_t cur_time) {
        cur_val_ = init_val_;
        elapsed_duration_ = 0;
        start_time_ = cur_time;
    }

    void Animator::setId(int id) {
        id_ = id;
    }

    void Animator::setRepeat(bool repeat) {
        is_repeat_ = repeat;
    }

    void Animator::setDuration(nsp duration) {
        duration_ = duration.count();
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

    void Animator::setFinalValue(double final_val) {
        final_val_ = final_val;
    }

    void Animator::setValueRange(double init_val, double final_val) {
        init_val_ = init_val;
        final_val_ = final_val;
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

    Animator::ns Animator::getDuration() const {
        return ns(duration_);
    }

    double Animator::getCurValue() const {
        return is_started_ ? cur_val_ : init_val_;
    }

    double Animator::getInitValue() const {
        return init_val_;
    }

    double Animator::getFinalValue() const {
        return final_val_;
    }

    Interpolator* Animator::getInterpolator() const {
        return interpolator_.get();
    }

    // static
    uint64_t Animator::now() {
        return utl::TimeUtils::upTimeNanos();
    }

}
