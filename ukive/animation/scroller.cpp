// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "scroller.h"

#include <algorithm>
#include <cmath>

#include "utils/log.h"

#include "ukive/event/input_consts.h"

#define NANO_RATIO   1000000000
#define NANO_RATIO_D 1000000000.0


namespace {

    /**
     * inertia 遵循的加速度。
     */
    const double kInertiaMU = 0.05;

    const double kBezierBaseTime = 6;
    const double kBezierBaseVelocity = 20000;

}

namespace ukive {

    Scroller::Scroller()
        : touch_curve_(
              kBezierBaseVelocity,
              kBezierBaseTime * 4,
              { 3, kBezierBaseVelocity },
              { 3, 0 }),
          pointer_curve_(
              kBezierBaseVelocity,
              kBezierBaseTime / 1.5,
              { 1, kBezierBaseVelocity },
              { 0, 0 })
    {
        // a = μg
        init_dec_ = kInertiaMU * (9.78 * 100 / 2.54);

        // bezier
        sf_ = 1;
        use_touch_curve_ = false;
    }

    Scroller::~Scroller() {
    }

    void Scroller::linear(int start, int distance, nsp duration) {
        if (distance == 0) {
            return;
        }

        delta_ = 0;
        prev_ = cur_ = start_ = start;
        type_ = LINEAR;

        bool finished = is_preparing_ || is_finished_;
        if (finished) {
            start_time_ = 0;
            is_preparing_ = true;
        } else {
            start_time_ = prev_time_;
        }

        is_finished_ = false;

        distance_ = distance;
        duration_ = duration.count();
    }

    void Scroller::inertia(float acc_scale, int start, float velocity) {
        if (velocity == 0) {
            return;
        }

        delta_ = 0;
        prev_ = cur_ = start_ = start;
        type_ = INERTIA;
        decelerate_ = init_dec_ * acc_scale;
        if (velocity >= 0) {
            decelerate_ *= -1;
        }

        bool finished =
            is_preparing_ || is_finished_ ||
            (cur_velocity_ * velocity < 0);
        if (finished) {
            cur_velocity_ = velocity;
            prev_time_ = start_time_ = 0;
            is_preparing_ = true;
        } else {
            cur_velocity_ += velocity;
            start_time_ = prev_time_;
        }

        cur_velocity_ = std::clamp(
            cur_velocity_, -kMaxVelocityInScroller, kMaxVelocityInScroller);
        is_finished_ = false;
    }

    void Scroller::bezier(int start, float velocity, bool is_touch) {
        if (velocity == 0) {
            return;
        }

        delta_ = 0;
        prev_ = cur_ = start_ = start;
        type_ = BEZIER;

        bool finished =
            is_preparing_ || is_finished_ ||
            (cur_velocity_ * velocity < 0) ||
            (use_touch_curve_ ^ is_touch);
        if (finished) {
            cur_velocity_ = velocity;
            prev_time_ = start_time_ = 0;
            is_preparing_ = true;
        } else {
            cur_velocity_ += velocity;
            start_time_ = prev_time_;
        }

        cur_velocity_ = std::clamp(
            cur_velocity_, -kMaxVelocityInScroller, kMaxVelocityInScroller);
        sf_ = cur_velocity_ / kBezierBaseVelocity;
        is_finished_ = false;
        use_touch_curve_ = is_touch;

        //DLOG(Log::INFO) << "Start bezier! st: " << start_time_ << " pt: " << prev_time_;
    }

    bool Scroller::compute(uint64_t cur_time, uint32_t display_freq) {
        if (is_finished_) {
            return false;
        }

        if (is_preparing_) {
            is_preparing_ = false;
            start_time_ = cur_time - (NANO_RATIO / display_freq);
            prev_time_ = start_time_;
        }

        switch (type_) {
        case LINEAR:
            computeLinear(cur_time);
            break;
        case INERTIA:
            computeInertia(cur_time);
            break;
        case BEZIER:
            computeBezier(cur_time);
            break;
        default:
            is_finished_ = true;
            break;
        }

        delta_ = cur_ - prev_;
        if (std::abs(delta_) >= 1) {
            prev_ = cur_;
        }
        prev_time_ = cur_time;

        return true;
    }

    void Scroller::computeLinear(uint64_t cur_time) {
        auto elapsed = (cur_time - start_time_) / NANO_RATIO_D;
        auto duration = duration_ / NANO_RATIO_D;

        if (elapsed >= duration) {
            cur_ = start_ + distance_;
            is_finished_ = true;
        } else {
            double percent = (std::min)(elapsed / duration, 1.0);
            cur_ = start_ + distance_ * percent;
        }
    }

    void Scroller::computeInertia(uint64_t cur_time) {
        double elapsed = (cur_time - start_time_) / NANO_RATIO_D;

        auto v1 = cur_velocity_ + decelerate_ * elapsed;
        if (v1 * cur_velocity_ < 0) {
            elapsed = -cur_velocity_ / decelerate_;
            v1 = 0;
        }
        cur_ = start_ + (cur_velocity_ + 0.5f * decelerate_ * elapsed) * elapsed;

        if (v1 == 0) {
            is_finished_ = true;
        }

        /*DLOG(Log::INFO) << "compute## elapsed:" << elapsed
            << " dt: " << dt
            << " velocity:" << cur_velocity_
            << " dy:" << (cur_ - prev_);*/
    }

    void Scroller::computeBezier(uint64_t cur_time) {
        auto dt = (cur_time - prev_time_) / NANO_RATIO_D;
        auto elapsed = (cur_time - start_time_) / NANO_RATIO_D;

        if (cur_velocity_ != 0) {
            double v;
            bool hit;
            if (use_touch_curve_) {
                hit = touch_curve_.cal(elapsed, sf_, &v);
            } else {
                hit = pointer_curve_.cal(elapsed, sf_, &v);
            }
            if (hit) {
                cur_ += cur_velocity_ * dt;
                cur_velocity_ = v;
            } else {
                cur_velocity_ = 0;
            }
        }

        if (cur_velocity_ == 0) {
            is_finished_ = true;
        }

        /*DLOG(Log::INFO) << "compute## dt:" << dt
            << " velocity:" << cur_velocity_
            << " dy:" << (cur_ - prev_);*/
    }

    void Scroller::finish() {
        is_preparing_ = false;
        is_finished_ = true;
        cur_velocity_ = 0;
    }

    bool Scroller::isFinished() const {
        return is_finished_;
    }

    int Scroller::getDelta() const {
        return int(delta_);
    }

    int Scroller::getCurScroll() const {
        return int(cur_);
    }

}
