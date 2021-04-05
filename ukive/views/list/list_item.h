// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_ITEM_H_
#define UKIVE_VIEWS_LIST_LIST_ITEM_H_

#include <cstddef>

#include "ukive/graphics/padding.hpp"


namespace ukive {

    class View;

    class ListItem {
    public:
        Margin ex_margins;
        View* item_view;
        int item_id;
        size_t data_pos;
        bool recycled;

        explicit ListItem(View* v);
        virtual ~ListItem();

        int getMgdLeft() const;
        int getMgdTop() const;
        int getMgdRight() const;
        int getMgdBottom() const;

        int getMgdWidth() const;
        int getMgdHeight() const;

        int getHoriMargins() const;
        int getVertMargins() const;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_ITEM_H_
