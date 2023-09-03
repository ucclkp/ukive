// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_BASICS_TOOLTIP_H_
#define UKIVE_BASICS_TOOLTIP_H_

#include "utils/time_utils.h"

#include "ukive/basics/levitator.h"
#include "ukive/animation/timer.h"


namespace ukive {

    class TextView;

    class Tooltip {
    public:
        Tooltip(Context c);

        void show(View* anchor, int gravity);
        void show(Window* host, int x, int y);
        void close();

        void setText(const std::u16string_view& text);
        void setTextSize(int size);
        void setTimeout(utl::TimeUtils::nsp timeout);

    private:
        Timer timer_;
        TextView* tv_ = nullptr;
        Levitator levitator_;
    };

}

#endif  // UKIVE_BASICS_TOOLTIP_H_