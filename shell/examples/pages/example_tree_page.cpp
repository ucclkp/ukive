// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "example_tree_page.h"

#include "utils/strings/int_conv.hpp"

#include "ukive/elements/element.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/image_view.h"
#include "ukive/views/text_view.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/tree/tree_node.h"
#include "ukive/views/tree/tree_node_button.h"
#include "ukive/resources/layout_parser.h"

#include "shell/resources/necro_resources_id.h"


namespace shell {

    // ExampleTreeItem
    ExampleTreeItem::ExampleTreeItem(ukive::View* v)
        : ListItem(v) {}

    ExampleTreePage::ExampleTreePage(ukive::Window* w)
        : Page(w) {}

    ukive::View* ExampleTreePage::onCreate(ukive::LayoutView* parent) {
        auto v = ukive::LayoutParser::from(
            parent->getContext(), parent, Res::Layout::example_tree_page_layout_xml);

        // ListView
        auto layouter = new ukive::LinearListLayouter();
        for (int i = 0; i < 10; ++i) {
            std::u16string str(u"test");
            str.append(utl::itos16(i));

            auto node = new ExampleTreeNode();
            node->text = str;
            root_node_.addNode(node);
        }

        list_view_ = findView<ukive::ListView>(v, Res::Id::lv_test_tree);
        list_view_->setLayouter(layouter);
        list_view_->setSource(this);
        list_view_->setSecDimUnknown(true);

        root_node_.setExpanded(true);

        return v;
    }

    void ExampleTreePage::onDestroy() {
    }

    ukive::ListItem* ExampleTreePage::onCreateListItem(
        ukive::LayoutView* parent, ukive::ListItemEventRouter* router, size_t position)
    {
        auto c = parent->getContext();

        auto layout = new ukive::RestraintLayout(c);
        layout->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);

        using Rlp = ukive::RestraintLayoutInfo;

        auto expand_btn = new ukive::TreeNodeButton(c);
        expand_btn->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
        expand_btn->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
        auto eb_lp = Rlp::Builder()
            .start(layout->getId())
            .top(layout->getId())
            .bottom(layout->getId())
            .build();
        expand_btn->setExtraLayoutInfo(eb_lp);
        layout->addView(expand_btn);

        auto avatar_view = new ukive::ImageView(c);
        avatar_view->setImage({});
        avatar_view->setLayoutSize(c.dp2pxi(12), c.dp2pxi(12));
        avatar_view->setLayoutMargin(c.dp2pxi(2), c.dp2pxi(2), 0, c.dp2pxi(2));
        Rlp* av_lp = new Rlp();
        av_lp->startHandle(expand_btn->getId(), Rlp::END);
        av_lp->topHandle(layout->getId(), Rlp::TOP);
        av_lp->bottomHandle(layout->getId(), Rlp::BOTTOM);
        avatar_view->setExtraLayoutInfo(av_lp);
        layout->addView(avatar_view);

        auto title_label = new ukive::TextView(c);
        title_label->setFontFamilyName(u"Consolas");
        title_label->setTextSize(c.dp2pxi(13));
        title_label->setClickable(true);
        title_label->setDoubleClickable(true);
        title_label->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
        title_label->setLayoutSize(ukive::View::LS_FILL, ukive::View::LS_AUTO);
        Rlp* tl_lp = new Rlp();
        tl_lp->startHandle(avatar_view->getId(), Rlp::END);
        tl_lp->endHandle(layout->getId(), Rlp::END);
        tl_lp->topHandle(layout->getId(), Rlp::TOP);
        tl_lp->bottomHandle(layout->getId(), Rlp::BOTTOM);
        title_label->setExtraLayoutInfo(tl_lp);
        layout->addView(title_label);

        auto root = new ExampleTreeItem(layout);
        root->avatar_image = avatar_view;
        root->text_label = title_label;
        root->expand_button_ = expand_btn;

        return root;
    }

    void ExampleTreePage::onSetListItemData(
        ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
        ukive::ListItem* item)
    {
        auto node = static_cast<ExampleTreeNode*>(
            root_node_.getExpandedDescendantAt(item->data_pos));
        auto tree_item = static_cast<ExampleTreeItem*>(item);

        tree_item->expand_button_->setOnClickListener(this);
        tree_item->text_label->setOnClickListener(this);

        tree_item->ex_margins.start(int(node->getLevel() * 16));
        tree_item->text_label->setText(node->text);

        if (node->getChildCount() == 0) {
            tree_item->expand_button_->setStatus(ukive::TreeNodeButton::NONE);
        }
        else {
            tree_item->expand_button_->setStatus(
                node->isExpanded() ? ukive::TreeNodeButton::EXPANDED : ukive::TreeNodeButton::COLLAPSED);
        }

        if (node->is_selected) {
            auto b = tree_item->text_label->getBackground();
            if (b) {
                b->setSolidColor(ukive::Color::Blue300);
            } else {
                tree_item->text_label->setBackground(new ukive::Element(ukive::Color::Blue300));
            }
        }
        else {
            auto b = tree_item->text_label->getBackground();
            if (b) {
                b->setSolidColor(ukive::Color::Transparent);
            }
        }

        //LOG(INFO) << "ListSource::onSetListItemData():" << position << " data has been bound.";
    }

    size_t ExampleTreePage::onGetListDataCount(ukive::LayoutView* parent) const {
        return root_node_.getExpandedDescendantCount();
    }

    void ExampleTreePage::onClick(ukive::View* v) {
        auto item = static_cast<ExampleTreeItem*>(list_view_->findItemFromView(v));
        if (v == item->expand_button_) {
            auto node = root_node_.getExpandedDescendantAt(item->data_pos);
            node->setExpanded(!node->isExpanded());
            notifyDataChanged();
        } else if (v == item->text_label) {
            auto node = static_cast<ExampleTreeNode*>(
                root_node_.getExpandedDescendantAt(item->data_pos));
            node->is_selected = true;
            if (selected_node_ && selected_node_ != node) {
                static_cast<ExampleTreeNode*>(selected_node_)->is_selected = false;
            }
            selected_node_ = node;
            notifyDataChanged();
        }
    }

    void ExampleTreePage::onDoubleClick(ukive::View* v) {
        auto item = static_cast<ExampleTreeItem*>(
            list_view_->findItemFromView(v));
        if (v == item->text_label) {
            auto node = root_node_.getExpandedDescendantAt(item->data_pos);
            node->setExpanded(!node->isExpanded());
            notifyDataChanged();
        }
    }

    void ExampleTreePage::onItemClicked(ukive::ListView* lv, ukive::ListItem* item) {
        auto node = root_node_.getExpandedDescendantAt(item->data_pos);
        if (node->isExpanded()) {
            node->setExpanded(false);
        } else {
            node->setExpanded(true);
            if (node->getChildCount() == 0) {
                auto child_node = new ExampleTreeNode();
                child_node->text = u"Label2";
                node->addNode(child_node);
            }
        }
        notifyDataChanged();
    }

}
