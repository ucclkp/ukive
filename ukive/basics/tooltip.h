// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_BASICS_TOOLTIP_H_
#define UKIVE_BASICS_TOOLTIP_H_

#include "utils/time_utils.h"

#include "ukive/basics/levitator.h"


namespace ukive {

    class Tooltip {
    public:
        Tooltip(Context c);

        void show(View* anchor, int gravity);
        void show(Window* host, int x, int y);
        void close();

        void setText(const std::u16string_view& text);
        void setTimeout(utl::TimeUtils::nsp timeout);

    private:
        Levitator levitator_;
    };

}

#endif  // UKIVE_BASICS_TOOLTIP_H_