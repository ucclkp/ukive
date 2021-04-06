// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "scroller.h"

#include <algorithm>
#include <cmath>

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/event/input_consts.h"
#include "ukive/system/time_utils.h"
#include "ukive/window/window.h"


namespace {

    /**
     * inertia 遵循的加速度。
     */
    const double kInertiaMU = 0.05;

    const double kBezierBaseTime = 6;
    const double kBezierBaseVelocity = 20000;

}

namespace ukive {

    Scroller::Scroller(Context c)
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
        // inertia
        auto dpi_x = c.getScale() * c.getDefaultDpi();
        init_dec_ = kInertiaMU * (9.78 * 100 / 2.54 * dpi_x);

        // bezier
        sf_ = 1;
        use_touch_curve_ = false;
    }

    Scroller::~Scroller() {
    }

    void Scroller::linear(int start, int distance, uint64_t duration) {
        if (distance == 0) {
            return;
        }

        delta_ = 0;
        prev_ = cur_ = start_ = start;
        type_ = LINEAR;

        // 当该动画正在运行时，开始时间将设置为上次增量的结束时间
        // 以防在开始的瞬间出现卡顿，下同。
        if (is_finished_) {
            start_time_ = TimeUtils::upTimeMicros();
        } else {
            start_time_ = prev_time_;
        }

        is_finished_ = false;

        distance_ = distance;
        duration_ = duration;
    }

    void Scroller::inertia(
        int start, float velocity, Continuity continuity)
    {
        if (velocity == 0) {
            return;
        }

        delta_ = 0;
        prev_ = cur_ = start_ = start;
        start_time_ = TimeUtils::upTimeMicros();
        type_ = INERTIA;
        decelerate_ = init_dec_;
        if (velocity >= 0) {
            decelerate_ *= -1;
        }

        bool should_not_continue =
            is_finished_ ||
            (cur_velocity_ * velocity < 0);

        switch (continuity) {
        case Continuity::None:
            cur_velocity_ = velocity;
            prev_time_ = start_time_;
            break;
        case Continuity::Time:
            cur_velocity_ = velocity;
            if (!should_not_continue) {
                start_time_ = prev_time_;
            } else {
                prev_time_ = start_time_;
            }
            break;
        case Continuity::VelocityAndTime:
            if (should_not_continue) {
                cur_velocity_ = velocity;
                prev_time_ = start_time_;
            } else {
                cur_velocity_ += velocity;
                start_time_ = prev_time_;
            }
            break;
        }

        cur_velocity_ = std::clamp(
            cur_velocity_, -kMaxVelocityInScroller, kMaxVelocityInScroller);
        is_finished_ = false;
    }

    void Scroller::bezier(
        int start, float velocity,
        Continuity continuity, bool is_touch)
    {
        if (velocity == 0) {
            return;
        }

        delta_ = 0;
        prev_ = cur_ = start_ = start;
        type_ = BEZIER;
        start_time_ = TimeUtils::upTimeMicros();

        bool should_not_continue =
            is_finished_ ||
            (cur_velocity_ * velocity < 0) ||
            (use_touch_curve_ ^ is_touch);

        switch (continuity) {
        case Continuity::None:
            cur_velocity_ = velocity;
            prev_time_ = start_time_;
            break;
        case Continuity::Time:
            cur_velocity_ = velocity;
            if (should_not_continue) {
                prev_time_ = start_time_;
            }
            break;
        case Continuity::VelocityAndTime:
            if (should_not_continue) {
                cur_velocity_ = velocity;
                prev_time_ = start_time_;
            } else {
                cur_velocity_ += velocity;
            }
            break;
        }

        cur_velocity_ = std::clamp(
            cur_velocity_, -kMaxVelocityInScroller, kMaxVelocityInScroller);
        sf_ = cur_velocity_ / kBezierBaseVelocity;
        is_finished_ = false;
        use_touch_curve_ = is_touch;

        //DLOG(Log::INFO) << "Start bezier! st: " << start_time_ << " pt: " << prev_time_;
    }

    bool Scroller::compute() {
        if (is_finished_) {
            return false;
        }

        auto cur_time = TimeUtils::upTimeMicros();

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
        auto elapsed = (cur_time - start_time_) / 1000.0;
        if (elapsed >= duration_) {
            cur_ = start_ + distance_;
            is_finished_ = true;
        } else {
            double percent = std::min(elapsed / double(duration_), 1.0);
            cur_ = start_ + distance_ * percent;
        }
    }

    void Scroller::computeInertia(uint64_t cur_time) {
        /*auto dt = (cur_time - prev_time_) / 1000000.0;
        if (Application::isVSyncEnabled() && dt < 1.0 / refresh_rate_) {
            cur_time = prev_time_ + 1000000 / refresh_rate_;
        }*/

        double elapsed = (cur_time - start_time_) / 1000000.0;

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
        auto dt = (cur_time - prev_time_) / 1000000.0;

        /**
         * 当前的滑动动画开始的时机没有和垂直信号进行同步，因此在触摸滑动的情况下，
         * 松手的瞬间启动动画的话，动画开始的时机很可能不是一帧的开始时间，就会在短时间出现卡顿。
         */
        auto elapsed = (cur_time - start_time_) / 1000000.0;

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
