// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/animation/timer.h"

#include "utils/weak_bind.hpp"


namespace ukive {

    Timer::Timer()
        : duration_(0),
          is_repeat_(false),
          is_running_(false),
          weak_ref_nest_(this) {}

    Timer::~Timer() {
        stop();
    }

    void Timer::start() {
        if (is_running_) {
            return;
        }
        postToMessageLoop();
        is_running_ = true;
    }

    void Timer::stop() {
        weak_ref_nest_.revoke();
        is_running_ = false;
    }

    void Timer::setRepeat(bool repeat) {
        is_repeat_ = repeat;
    }

    void Timer::setRunner(const Runner& runner) {
        runner_ = runner;
    }

    void Timer::setDuration(nsp duration) {
        duration_ = duration;
    }

    bool Timer::isRepeat() const {
        return is_repeat_;
    }

    bool Timer::isRunning() const {
        return is_running_;
    }

    Timer::nsp Timer::getDuration() const {
        return duration_;
    }

    void Timer::onTimer() {
        if (runner_) {
            runner_();
        }

        if (is_repeat_) {
            if (is_running_) {
                postToMessageLoop();
            }
        } else {
            is_running_ = false;
        }
    }

    void Timer::postToMessageLoop() {
        auto callback = weakref_bind(&Timer::onTimer, weak_ref_nest_.getRef());
        cycler_.postDelayed(callback, duration_);
    }

}