// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_ITEM_RECYCLER_H_
#define UKIVE_VIEWS_LIST_LIST_ITEM_RECYCLER_H_

#include <map>
#include <vector>

#include "ukive/views/list/list_source.h"


namespace ukive {

    class View;
    class LayoutView;

    class ListItemRecycler {
    public:
        explicit ListItemRecycler(LayoutView* parent);

        void addToParent(ListItem* item);
        void addToParent(ListItem* item, int pos);
        void addToRecycler(ListItem* item);
        void recycleFromParent(ListItem* item);
        ListItem* reuse(int item_id);
        ListItem* reuse(int item_id, int pos);
        int getRecycledCount(int item_id);
        void clear();

    private:
        LayoutView* parent_;
        std::map<int, std::vector<ListItem*>> recycled_items_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_ITEM_RECYCLER_H_