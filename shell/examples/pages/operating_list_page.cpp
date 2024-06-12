// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "operating_list_page.h"

#include "utils/strings/int_conv.hpp"

#include "ukive/views/button.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/list/flow_list_layouter.h"
#include "ukive/views/list/grid_list_layouter.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/seek_bar.h"
#include "ukive/views/image_view.h"
#include "ukive/window/window.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/system/dialogs/sys_message_dialog.h"
#include "ukive/dialogs/message_dialog.h"

#include "shell/resources/necro_resources_id.h"


namespace shell {

    OperatingListItem::OperatingListItem(ukive::View* v)
        : ListItem(v)
    {
        title_label = static_cast<ukive::TextView*>(v->findView(ID_TITLE));
        seek_bar = static_cast<ukive::SeekBar*>(v->findView(ID_SEEKBAR));
    }

    ukive::ListItem* OperatingListPage::onCreateListItem(
        ukive::LayoutView* parent, ukive::ListItemEventRouter* router, size_t position)
    {
        auto layout = new ukive::RestraintLayout(parent->getContext());
        layout->setBackground(new ukive::Element(ukive::Color::Blue100));
        layout->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        //layout->setLayoutMargin(4, 4, 4, 4);

        using Rlp = ukive::RestraintLayoutInfo;

        auto title_label = new ukive::TextView(parent->getContext());
        title_label->setId(ID_TITLE);
        title_label->autoWrap(true);
        title_label->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        title_label->setLayoutMargin(8, 8, 8, 8);
        Rlp* sl_lp = Rlp::Builder()
            .start(layout->getId())
            .top(layout->getId())
            .end(layout->getId()).build();
        title_label->setExtraLayoutInfo(sl_lp);
        layout->addView(title_label);

        auto seek_bar = new ukive::SeekBar(parent->getContext());
        seek_bar->setId(ID_SEEKBAR);
        seek_bar->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        seek_bar->setLayoutMargin(8, 8, 8, 8);
        Rlp* tl_lp = Rlp::Builder()
            .start(layout->getId())
            .top(title_label->getId(), Rlp::BOTTOM)
            .end(layout->getId())
            .bottom(layout->getId()).build();
        seek_bar->setExtraLayoutInfo(tl_lp);
        layout->addView(seek_bar);

        return new OperatingListItem(layout);
    }

    void OperatingListPage::onSetListItemData(
        ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
        ukive::ListItem* item)
    {
        auto& data = data_list_.at(item->data_pos);
        OperatingListItem* op_list_item = static_cast<OperatingListItem*>(item);

        std::u16string str(data.title);
        str.append(u" ").append(utl::itos16(item->data_pos));

        op_list_item->title_label->setText(str);
        op_list_item->seek_bar->setProgress(data.progress);
    }

    size_t OperatingListPage::onGetListDataCount(ukive::LayoutView* parent) const {
        return data_list_.size();
    }

    OperatingListPage::OperatingListPage()
        : Page() {}

    ukive::View* OperatingListPage::onCreate(ukive::Context c) {
        auto v = ukive::LayoutParser::from(
            c, nullptr, Res::Layout::operating_list_page_layout_xml);

        // Buttons
        op_button_ = findView<ukive::Button>(v, Res::Id::bt_op_button);
        op_button_->setOnClickListener(this);

        state_label_ = findView<ukive::TextView>(v, Res::Id::tv_stats);

        // ListView
        for (int i = 0; i < 2; ++i) {
            data_list_.push_back({ u"test", 100.f});
        }

        list_view_ = findView<ukive::ListView>(v, Res::Id::lv_op_list_page_list);
        //list_view_->setLayouter(new ukive::GridListLayouter(4));
        list_view_->setLayouter(new ukive::LinearListLayouter());
        //list_view_->setLayouter(new ukive::FlowListLayouter(4));
        list_view_->setSource(this);

        using namespace std::chrono_literals;
        timer_.setRepeat(true);
        timer_.setDuration(30ms);
        timer_.setRunner([this]()
        {
                state_label_->setText(u"状态");
        });

        return v;
    }

    void OperatingListPage::onDestroy() {
    }

    void OperatingListPage::onClick(ukive::View* v) {
        if (v == op_button_) {
            timer_.start();
        }
    }

}
