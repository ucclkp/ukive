// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_item.h"

#include "ukive/views/view.h"


namespace ukive {

    // ListItem
    ListItem::ListItem(View* v)
        : item_view(v),
        item_id(-1),
        data_pos(-1),
        recycled(false) {
    }

    ListItem::~ListItem() {
        if (recycled) {
            delete item_view;
        }
    }

    int ListItem::getMgdLeft() const {
        return item_view->getLeft() -
            item_view->getLayoutMargin().start - ex_margins.left;
    }

    int ListItem::getMgdTop() const {
        return item_view->getTop() -
            item_view->getLayoutMargin().top - ex_margins.top;
    }

    int ListItem::getMgdRight() const {
        return item_view->getRight() +
            item_view->getLayoutMargin().end + ex_margins.right;
    }

    int ListItem::getMgdBottom() const {
        return item_view->getBottom() +
            item_view->getLayoutMargin().bottom + ex_margins.bottom;
    }

    int ListItem::getMgdWidth() const {
        return item_view->getWidth() + getHoriMargins();
    }

    int ListItem::getMgdHeight() const {
        return item_view->getHeight() + getVertMargins();
    }

    int ListItem::getHoriMargins() const {
        return item_view->getLayoutMargin().hori() +
            ex_margins.left + ex_margins.right;
    }

    int ListItem::getVertMargins() const {
        return item_view->getLayoutMargin().vert() +
            ex_margins.top + ex_margins.bottom;
    }

}