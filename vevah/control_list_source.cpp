// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "control_list_source.h"

#include "ukive/views/list/list_item.h"
#include "ukive/views/text_view.h"
#include "ukive/views/button.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/list/list_item_event_router.h"


namespace {

    class CtrlListItem : public ukive::ListItem {
    public:
        explicit CtrlListItem(ukive::View* v)
            : ListItem(v) {}

        ukive::TextView* title_ = nullptr;
    };

}

namespace vevah {

    ControlListSource::ControlListSource() {
        data_.push_back({ u"Button", "Button" });
        data_.push_back({ u"TextViiiiii", "TextView" });
    }

    ukive::ListItem* ControlListSource::onCreateListItem(
        ukive::LayoutView* parent,
        ukive::ListItemEventRouter* router, size_t position)
    {
        auto bt = new ukive::Button(parent->getContext());
        bt->setShadowRadius(0);
        bt->setOnClickListener(router);
        bt->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);

        auto item = new CtrlListItem(bt);
        item->title_ = bt;
        return item;
    }

    void ControlListSource::onSetListItemData(
        ukive::LayoutView* parent,
        ukive::ListItemEventRouter* router, ukive::ListItem* item)
    {
        auto& data = data_[item->data_pos];
        auto ctrl_item = static_cast<CtrlListItem*>(item);
        ctrl_item->title_->setText(data.text);
    }

    size_t ControlListSource::onGetListDataCount(ukive::LayoutView* parent) const {
        return data_.size();
    }

}
