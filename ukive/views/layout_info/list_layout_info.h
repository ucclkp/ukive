// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_LIST_LAYOUT_INFO_H_
#define UKIVE_VIEWS_LAYOUT_INFO_LIST_LAYOUT_INFO_H_

#include <memory>

#include "ukive/graphics/padding.hpp"
#include "ukive/views/layout_info/layout_info.h"


namespace ukive {

    class ListLayoutInfo : public LayoutInfo {
    public:
        ListLayoutInfo();

        int id = -1;
        int position = -1;
        bool recycled = false;
        Margin margins;

        std::shared_ptr<void> data;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_LAYOUT_INFO_H_