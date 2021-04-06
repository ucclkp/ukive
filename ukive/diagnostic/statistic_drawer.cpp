// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "statistic_drawer.h"

#include <cmath>

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/display.h"
#include "ukive/window/window.h"


namespace ukive {

    StatisticDrawer::StatisticDrawer(Context c)
        : mode_(RENDER),
          strip_width_(c.dp2pxi(4)),
          context_(c) {}

    void StatisticDrawer::draw(int x, int y, int width, int height, Canvas* canvas) {
        int cur_x = x + width;
        int base_height = context_.dp2pxi(64);
        float base_time = 0.f;
        if (mode_ == LAYOUT) {
            base_time = 4.f;
        } else {
            base_time = 100.f / 6.f;
        }

        for (auto it = durations_.rbegin(); it != durations_.rend(); ++it) {
            int top = int(y + height - it->duration / base_time * base_height);
            canvas->fillRect(
                RectF(Rect(cur_x - strip_width_, top, strip_width_, y + height - top)),
                mode_ == RENDER ? Color::Orange400 : Color::Pink200);

            cur_x -= strip_width_;
            if (cur_x < 0) {
                durations_.erase(durations_.begin(), it.base());
                break;
            }
        }

        Rect base_line(x, y + height - base_height, width, int(std::round(context_.dp2px(2))));
        canvas->fillRect(RectF(base_line), Color::Red400);
    }

    void StatisticDrawer::toggleMode() {
        if (mode_ == RENDER) {
            mode_ = LAYOUT;
        } else if (mode_ == LAYOUT) {
            mode_ = RENDER;
        }

        durations_.clear();
    }

    void StatisticDrawer::addDuration(uint64_t duration) {
        durations_.push_back(FrameDuration(duration));
    }

    StatisticDrawer::Mode StatisticDrawer::getMode() {
        return mode_;
    }
}
