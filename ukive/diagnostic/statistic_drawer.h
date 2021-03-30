// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIAGNOSTIC_STATISTIC_DRAWER_H_
#define UKIVE_DIAGNOSTIC_STATISTIC_DRAWER_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class Window;

    class StatisticDrawer {
    public:
        enum Mode {
            RENDER,
            LAYOUT
        };

        explicit StatisticDrawer(Context c);

        void toggleMode();
        void addDuration(uint64_t duration);

        Mode getMode();

        void draw(int x, int y, int width, int height, Canvas* canvas);

    private:
        struct FrameDuration {
            float duration;

            explicit FrameDuration(uint64_t micro)
                : duration(micro / 1000.f) {}
        };

        Mode mode_;
        int strip_width_;
        Context context_;
        std::vector<FrameDuration> durations_;
    };

}

#endif  // UKIVE_DIAGNOSTIC_STATISTIC_DRAWER_H_