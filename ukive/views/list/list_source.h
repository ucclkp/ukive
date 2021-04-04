// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_SOURCE_H_
#define UKIVE_VIEWS_LIST_LIST_SOURCE_H_

#include <cstddef>


namespace ukive {

    class View;
    class LayoutView;
    class ListItem;
    class ListItemEventRouter;

    class ListItemChangedNotifier {
    public:
        virtual ~ListItemChangedNotifier() = default;

        virtual void onDataChanged() = 0;
        virtual void onItemInserted(size_t start_pos, size_t count) = 0;
        virtual void onItemChanged(size_t start_pos, size_t count) = 0;
        virtual void onItemRemoved(size_t start_pos, size_t count) = 0;
    };

    class ListSource {
    public:
        ListSource();
        virtual ~ListSource() = default;

        void setNotifier(ListItemChangedNotifier* n) { notifier_ = n; }

        void notifyDataChanged();
        void notifyItemChanged(size_t start_pos, size_t count);
        void notifyItemInserted(size_t start_pos, size_t count);
        void notifyItemRemoved(size_t start_pos, size_t count);

        virtual ListItem* onCreateListItem(
            LayoutView* parent, ListItemEventRouter* router, size_t position) = 0;
        virtual void onSetListItemData(
            LayoutView* parent, ListItemEventRouter* router, ListItem* item) = 0;
        virtual int onGetListItemId(
            LayoutView* parent, size_t position) const { return 0; }
        virtual size_t onGetListDataCount(LayoutView* parent) const = 0;

    private:
        ListItemChangedNotifier* notifier_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_SOURCE_H_