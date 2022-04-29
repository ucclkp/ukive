// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "anitom.h"

#include "interpolator.h"


namespace ukive {

    Anitom::Anitom() {}

    Anitom::~Anitom() {}

    void Anitom::setId(int id) {
        id_ = id;
    }

    void Anitom::setStartTime(nsp start) {
        start_time_ = start;
    }

    void Anitom::setInevitable(bool inevitable) {
        is_inevitable_ = inevitable;
    }

    void Anitom::setDuration(nsp duration) {
        duration_ = duration;
    }

    void Anitom::setStarted(bool started) {
        is_started_ = started;
    }

    void Anitom::setRunning(bool running) {
        is_running_ = running;
    }

    void Anitom::setFinished(bool finished) {
        is_finished_ = finished;
    }

    void Anitom::setInitValue(double val) {
        init_val_ = val;
    }

    void Anitom::setFinalValue(double val) {
        final_val_ = val;
    }

    void Anitom::setInterpolator(Interpolator* i) {
        if (!i || interpolator_.get() == i) {
            return;
        }
        interpolator_.reset(i);
    }

    int Anitom::getId() const {
        return id_;
    }

    Anitom::ns Anitom::getStartTime() const {
        return start_time_;
    }

    Anitom::ns Anitom::getEndTime() const {
        return start_time_ + duration_;
    }

    Anitom::ns Anitom::getDuration() const {
        return duration_;
    }

    double Anitom::getInitValue() const {
        return init_val_;
    }

    double Anitom::getFinalValue() const {
        return final_val_;
    }

    const Interpolator* Anitom::getInterpolator() const {
        return interpolator_.get();
    }

    double Anitom::getCurValue(nsp cur_time) const {
        if (cur_time <= start_time_) {
            return init_val_;
        }
        if (cur_time >= start_time_ + duration_) {
            return final_val_;
        }

        double progress = double((cur_time - start_time_).count()) / duration_.count();
        return interpolator_->interpolate(init_val_, final_val_, progress);
    }

    bool Anitom::isStarted() const {
        return is_started_;
    }

    bool Anitom::isRunning() const {
        return is_running_;
    }

    bool Anitom::isFinished() const {
        return is_finished_;
    }

    bool Anitom::isInevitable() const {
        return is_inevitable_;
    }

}
