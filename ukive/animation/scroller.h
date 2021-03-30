// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_SCROLLER_H_
#define UKIVE_ANIMATION_SCROLLER_H_

#include <cstdint>

#include "ukive/animation/bezier_curve.h"


namespace ukive {

    class Context;

    class Scroller {
    public:
        enum class Continuity {
            None,
            Time,
            VelocityAndTime,
        };

        explicit Scroller(Context c);
        ~Scroller();

        /**
         * 线性动画，对应匀速运动。
         * @param start 表示起始值
         * @param distance 表示要经历的值段
         * @param duration 表示经历 distance 所花的时间
         */
        void linear(int start, int distance, uint64_t duration);

        /**
         * 抛物线动画，将由起始速度根据特定加速度减速到零。
         * @param start 表示起始值
         * @param velocity 表示起始速度
         * @param continuity 表示延续模式
         */
        void inertia(
            int start, float velocity, Continuity continuity);

        /**
         * 近似三次贝塞尔曲线动画，参见 BezierCurve。
         * @param start 表示起始值
         * @param velocity 表示起始速度
         * @param continuity 表示延续模式
         * @param is_touch 指示是否是由触摸触发的滚动
         */
        void bezier(
            int start, float velocity,
            Continuity continuity, bool is_touch);

        bool compute();
        void finish();

        bool isFinished() const;
        int getDelta() const;
        int getCurScroll() const;

    private:
        enum Type {
            INERTIA,
            LINEAR,
            BEZIER,
        };

        void computeLinear(uint64_t cur_time);
        void computeInertia(uint64_t cur_time);
        void computeBezier(uint64_t cur_time);

        double cur_ = 0;
        double prev_ = 0;
        double delta_ = 0;
        bool is_finished_ = true;

        int start_ = 0;
        uint64_t start_time_ = 0;
        uint64_t prev_time_ = 0;
        double cur_velocity_ = 0;
        Type type_ = LINEAR;

        // Linear
        int distance_ = 0;
        uint64_t duration_ = 0;

        // inertia
        double init_dec_ = 0;
        double decelerate_ = 0;

        // bezier
        double sf_;
        bool use_touch_curve_;
        BezierCurve touch_curve_;
        BezierCurve pointer_curve_;
    };

}

#endif  // UKIVE_ANIMATION_SCROLLER_H_