// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_HANDLER_SOURCE_H_
#define UKIVE_VIEWS_LIST_LIST_HANDLER_SOURCE_H_

#include <cstddef>
#include <functional>

#include "ukive/views/list/list_source.h"


namespace ukive {

    class ListHandlerSource : public ListSource {
    public:
        using OnItemHandler = std::function<
            ListItem*(LayoutView* parent, ListItemEventRouter* router, size_t position)>;
        using OnItemDataHandler = std::function<
            void(LayoutView* parent, ListItemEventRouter* router, ListItem* item)>;
        using OnItemIdHandler = std::function<int(LayoutView* parent, size_t position)>;
        using OnDataCountHandler = std::function<size_t(LayoutView* parent)>;

        ListHandlerSource();

        void setItemHandler(const OnItemHandler& h);
        void setItemDataHandler(const OnItemDataHandler& h);
        void setItemIdHandler(const OnItemIdHandler& h);
        void setDataCountHandler(const OnDataCountHandler& h);

        ListItem* onCreateListItem(
            LayoutView* parent, ListItemEventRouter* router, size_t position) override;
        void onSetListItemData(
            LayoutView* parent, ListItemEventRouter* router, ListItem* item) override;
        int onGetListItemId(
            LayoutView* parent, size_t position) const override;
        size_t onGetListDataCount(LayoutView* parent) const override;

    private:
        OnItemHandler item_handler_;
        OnItemDataHandler item_data_handler_;
        OnItemIdHandler item_id_handler_;
        OnDataCountHandler data_count_handler_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_HANDLER_SOURCE_H_