// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_CONTROL_LIST_ADAPTER_H_
#define VEVAH_CONTROL_LIST_ADAPTER_H_

#include "ukive/views/list/list_source.h"

#include <string>
#include <vector>


namespace vevah {

    class ControlListSource :
        public ukive::ListSource
    {
    public:
        ControlListSource();

        ukive::ListItem* onCreateListItem(
            ukive::LayoutView* parent,
            ukive::ListItemEventRouter* router, size_t position) override;
        void onSetListItemData(
            ukive::LayoutView* parent,
            ukive::ListItemEventRouter* router, ukive::ListItem* item) override;
        size_t onGetListDataCount(ukive::LayoutView* parent) const override;

        const std::string& getName(size_t dpos) const;

    private:
        struct ItemData {
            std::u16string text;
            std::string name;
        };

        std::vector<ItemData> data_;
    };

}

#endif  // VEVAH_CONTROL_LIST_ADAPTER_H_