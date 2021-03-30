// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_EVENT_VELOCITY_CALCULATOR_H_
#define UKIVE_EVENT_VELOCITY_CALCULATOR_H_

#include <cstdint>
#include <list>

#include "ukive/animation/timer.h"


namespace ukive {

    class InputEvent;

    class VelocityCalculator {
    public:
        VelocityCalculator();

        void onInputEvent(const InputEvent* e);

        /**
         * 获取当前计算的 x 轴方向上的速度。
         * 单位为像素/秒。
         */
        float getVelocityX() const;

        /**
         * 获取当前计算的 y 轴方向上的速度。
         * 单位为像素/秒。
         */
        float getVelocityY() const;

    private:
        struct Record {
            int dx;
            int dy;
            int duration;
        };

        void onRun();
        bool calculate();

        int cur_x_ = 0;
        int cur_y_ = 0;
        int prev_x_ = 0;
        int prev_y_ = 0;
        int start_x_ = 0;
        int start_y_ = 0;
        bool is_touch_down_ = false;
        bool is_touch_move_ = false;

        uint64_t prev_time_ = 0;
        int total_duration_ = 0;
        double velocity_x_ = 0;
        double velocity_y_ = 0;

        std::list<Record> history_;
    };

}

#endif  // UKIVE_EVENT_VELOCITY_CALCULATOR_H_