// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_ITEM_EVENT_ROUTER_H_
#define UKIVE_VIEWS_LIST_LIST_ITEM_EVENT_ROUTER_H_

#include "ukive/views/click_listener.h"
#include "ukive/views/view_delegate.h"


namespace ukive {

    class ListView;
    class ListItem;

    class ListItemEventListener {
    public:
        virtual ~ListItemEventListener() = default;

        virtual void onItemPressed(ListView* list_view, ListItem* item, View* v) {}
        virtual void onItemClicked(ListView* list_view, ListItem* item, View* v) {}
        virtual void onItemDoubleClicked(ListView* list_view, ListItem* item, View* v) {}
    };

    class ListItemEventRouter :
        public OnClickListener,
        public ViewDelegate
    {
    public:
        explicit ListItemEventRouter(ListItemEventListener* listener);

        void setListView(ListView* lv);

        // OnClickListener
        void onClick(View* v) override;
        void onDoubleClick(View* v) override;

        // ViewDelegate
        bool onInputReceived(View* v, InputEvent* e, bool* ret) override;

    private:
        ListView* list_view_ = nullptr;
        ListItemEventListener* listener_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_ITEM_EVENT_ROUTER_H_