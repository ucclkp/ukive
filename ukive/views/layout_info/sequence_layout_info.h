// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_SEQUENCE_LAYOUT_INFO_H_
#define UKIVE_VIEWS_LAYOUT_INFO_SEQUENCE_LAYOUT_INFO_H_

#include "ukive/views/layout_info/layout_info.h"


namespace ukive {

    class SequenceLayoutInfo : public LayoutInfo {
    public:
        enum class Position {
            START,
            CENTER,
            END,
        };

        SequenceLayoutInfo();

        int weight = 0;
        Position vertical_position = Position::START;
        Position horizontal_position = Position::START;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_SEQUENCE_LAYOUT_INFO_H_