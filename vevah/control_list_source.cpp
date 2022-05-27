// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "control_list_source.h"

#include "utils/strings/string_utils.hpp"

#include "ukive/resources/layout_parser.h"
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
        auto& map = ukive::LayoutParser::getViewMap();
        auto& map2 = ukive::LayoutParser::getViewMap2();

        // 预设非容器
        for (const auto& pair : map) {
            if (!pair.second.is_layout) {
                if (pair.first != "View") {
                    data_.push_back(
                        { utl::UTF8ToUTF16(pair.first), pair.first });
                }
            }
        }

        // 自定义非容器
        for (const auto& pair : map2) {
            if (!pair.second.is_layout) {
                data_.push_back(
                    { utl::UTF8ToUTF16(pair.first), pair.first });
            }
        }

        // 预设容器
        for (const auto& pair : map) {
            if (pair.second.is_layout) {
                if (pair.first != "LayoutView") {
                    data_.push_back(
                        { utl::UTF8ToUTF16(pair.first), pair.first });
                }
            }
        }

        // 自定义容器
        for (const auto& pair : map2) {
            if (pair.second.is_layout) {
                if (pair.first != "vevah::ContainerLayout") {
                    data_.push_back(
                        { utl::UTF8ToUTF16(pair.first), pair.first });
                }
            }
        }
    }

    ukive::ListItem* ControlListSource::onCreateListItem(
        ukive::LayoutView* parent,
        ukive::ListItemEventRouter* router, size_t position)
    {
        auto bt = new ukive::Button(parent->getContext());
        bt->setShadowRadius(0);
        bt->setOnClickListener(router);
        bt->setOnInputEventDelegate(router);
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

    const std::string& ControlListSource::getName(size_t dpos) const {
        return data_[dpos].name;
    }

}
