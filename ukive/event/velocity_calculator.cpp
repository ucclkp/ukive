// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/event/velocity_calculator.h"

#include <algorithm>

#include "utils/number.hpp"
#include "utils/log.h"

#include "ukive/event/input_consts.h"
#include "ukive/event/input_event.h"
#include "ukive/system/time_utils.h"


namespace ukive {

    VelocityCalculator::VelocityCalculator() {
    }

    void VelocityCalculator::onInputEvent(const InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVT_DOWN:
            //DLOG(Log::INFO) << "VelocityCalculator::EVT_DOWN";
            is_touch_down_ = true;
            cur_x_ = e->getX();
            cur_y_ = e->getY();
            start_x_ = prev_x_ = cur_x_;
            start_y_ = prev_y_ = cur_y_;
            velocity_x_ = 0;
            velocity_y_ = 0;
            prev_time_ = TimeUtils::upTimeMillisPrecise();
            history_.clear();
            break;

        case InputEvent::EVT_MOVE:
        {
            cur_x_ = e->getX();
            cur_y_ = e->getY();

            if (!is_touch_move_) {
                prev_x_ = cur_x_;
                prev_y_ = cur_y_;
                prev_time_ = TimeUtils::upTimeMillisPrecise();
                is_touch_move_ = true;
                total_duration_ = 0;
                history_.clear();
            } else {
                onRun();
            }
            break;
        }

        case InputEvent::EVT_UP:
        {
            //DLOG(Log::INFO) << "VelocityCalculator::EVT_UP";
            calculate();
            is_touch_down_ = false;
            is_touch_move_ = false;
            break;
        }

        default:
            break;
        }
    }

    void VelocityCalculator::onRun() {
        uint64_t cur_time = TimeUtils::upTimeMillisPrecise();
        if (cur_time > prev_time_) {
            Record record;
            record.dx = cur_x_ - prev_x_;
            record.dy = cur_y_ - prev_y_;
            record.duration = utl::num_cast<int>(cur_time - prev_time_);
            history_.push_front(record);

            total_duration_ += record.duration;

            if (total_duration_ > 128) {
                total_duration_ -= history_.back().duration;
                history_.pop_back();
            }
            prev_x_ = cur_x_;
            prev_y_ = cur_y_;
        }
        prev_time_ = cur_time;
    }

    bool VelocityCalculator::calculate() {
        int dx = 0;
        int dy = 0;
        int tx = 0;
        int ty = 0;
        bool x_co = false;
        bool y_co = false;
        for (auto& r : history_) {
            if (!x_co && std::abs(dx + r.dx) < std::abs(dx)) {
                x_co = true;
            }
            if (!y_co && std::abs(dy + r.dy) < std::abs(dy)) {
                y_co = true;
            }
            if (!x_co) {
                dx += r.dx; tx += r.duration;
            }
            if (!y_co) {
                dy += r.dy; ty += r.duration;
            }
        }

        if (tx == 0) {
            velocity_x_ = 0;
        } else {
            velocity_x_ = double(dx) / tx * 1000;
            velocity_x_ = std::clamp(
                velocity_x_, -kMaxVelocityInCal, kMaxVelocityInCal);
        }

        if (ty == 0) {
            velocity_y_ = 0;
        } else {
            velocity_y_ = double(dy) / ty * 1000;
            velocity_y_ = std::clamp(
                velocity_y_, -kMaxVelocityInCal, kMaxVelocityInCal);
        }

        return true;
    }

    float VelocityCalculator::getVelocityX() const {
        //LOG(Log::INFO) << "######## VX: " << float(velocity_x_);
        return float(velocity_x_);
    }

    float VelocityCalculator::getVelocityY() const {
        //LOG(Log::INFO) << "######## VY: " << float(velocity_y_);
        return float(velocity_y_);
    }

}