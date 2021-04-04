// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_item_event_router.h"

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_layouter.h"


namespace ukive {

    ListItemEventRouter::ListItemEventRouter(ListItemEventListener* listener)
        : listener_(listener) {}

    void ListItemEventRouter::setListView(ListView* lv) {
        list_view_ = lv;
    }

    void ListItemEventRouter::onClick(View* v) {
        if (listener_ && list_view_) {
            auto item = list_view_->findItemFromView(v);
            if (item) {
                listener_->onItemClicked(list_view_, item, v);
            }
        }
    }

    void ListItemEventRouter::onDoubleClick(View* v) {
        if (listener_ && list_view_) {
            auto item = list_view_->findItemFromView(v);
            if (item) {
                listener_->onItemDoubleClicked(list_view_, item, v);
            }
        }
    }

    bool ListItemEventRouter::onInputReceived(View* v, InputEvent* e, bool* ret) {
        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
        case InputEvent::EVT_DOWN:
            if (listener_ && list_view_) {
                auto item = list_view_->findItemFromView(v);
                if (item) {
                    listener_->onItemPressed(list_view_, item, v);
                }
            }
            break;

        default:
            break;
        }
        return false;
    }

}
