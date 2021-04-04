// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_LIST_LAYOUT_INFO_H_
#define UKIVE_VIEWS_LAYOUT_INFO_LIST_LAYOUT_INFO_H_

#include "ukive/views/layout_info/layout_info.h"


namespace ukive {

    class ListItem;

    class ListLayoutInfo : public LayoutInfo {
    public:
        ListLayoutInfo();

        ListItem* item = nullptr;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_LAYOUT_INFO_H_