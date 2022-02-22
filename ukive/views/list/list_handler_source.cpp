// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_handler_source.h"


namespace ukive {

    ListHandlerSource::ListHandlerSource() {}

    void ListHandlerSource::setItemHandler(const OnItemHandler& h) {
        item_handler_ = h;
    }

    void ListHandlerSource::setItemDataHandler(const OnItemDataHandler& h) {
        item_data_handler_ = h;
    }

    void ListHandlerSource::setItemIdHandler(const OnItemIdHandler& h) {
        item_id_handler_ = h;
    }

    void ListHandlerSource::setDataCountHandler(const OnDataCountHandler& h) {
        data_count_handler_ = h;
    }

    ListItem* ListHandlerSource::onCreateListItem(
        LayoutView* parent, ListItemEventRouter* router, size_t position)
    {
        if (item_handler_) {
            return item_handler_(parent, router, position);
        }
        return nullptr;
    }

    void ListHandlerSource::onSetListItemData(
        LayoutView* parent, ListItemEventRouter* router, ListItem* item)
    {
        if (item_data_handler_) {
            item_data_handler_(parent, router, item);
        }
    }

    int ListHandlerSource::onGetListItemId(
        LayoutView* parent, size_t position) const
    {
        if (item_id_handler_) {
            return item_id_handler_(parent, position);
        }
        return 0;
    }

    size_t ListHandlerSource::onGetListDataCount(LayoutView* parent) const {
        if (data_count_handler_) {
            return data_count_handler_(parent);
        }
        return 0;
    }

}