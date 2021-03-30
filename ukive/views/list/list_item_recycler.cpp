// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_item_recycler.h"

#include "utils/number.hpp"
#include "utils/log.h"
#include "utils/stl_utils.h"

#include "ukive/views/layout/layout_view.h"
#include "ukive/views/list/list_item.h"


namespace ukive {

    ListItemRecycler::ListItemRecycler(LayoutView* parent)
        :parent_(parent) {}

    void ListItemRecycler::addToParent(ListItem* item) {
        DCHECK(item);

        item->recycled = false;
        parent_->addView(item->item_view, false);
    }

    void ListItemRecycler::addToParent(ListItem* item, int pos) {
        DCHECK(item && pos >= 0);

        item->recycled = false;
        parent_->addView(pos, item->item_view, false);
    }

    void ListItemRecycler::addToRecycler(ListItem* item) {
        DCHECK(item);

        item->recycled = true;
        item->data_pos = -1;
        recycled_items_[item->item_id].push_back(item);
    }

    void ListItemRecycler::recycleFromParent(ListItem* item) {
        DCHECK(item);

        item->recycled = true;
        item->data_pos = -1;
        recycled_items_[item->item_id].push_back(item);
        parent_->removeView(item->item_view, false, false);
    }

    ListItem* ListItemRecycler::reuse(int item_id) {
        if (recycled_items_[item_id].empty()) {
            return nullptr;
        }

        auto item = recycled_items_[item_id].back();
        recycled_items_[item_id].pop_back();

        addToParent(item);

        return item;
    }

    ListItem* ListItemRecycler::reuse(int item_id, int pos) {
        DCHECK(pos >= 0);

        if (recycled_items_[item_id].empty()) {
            return nullptr;
        }

        auto item = recycled_items_[item_id].back();
        recycled_items_[item_id].pop_back();

        addToParent(item, pos);

        return item;
    }

    int ListItemRecycler::getRecycledCount(int item_id) {
        return utl::num_cast<int>(recycled_items_[item_id].size());
    }

    void ListItemRecycler::clear() {
        for (auto& pair : recycled_items_) {
            utl::STLDeleteElements(&pair.second);
        }

        recycled_items_.clear();
    }

}