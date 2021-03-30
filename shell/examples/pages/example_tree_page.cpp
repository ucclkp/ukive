// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "example_tree_page.h"

#include "utils/convert.h"

#include "ukive/views/tree/tree_source.h"
#include "ukive/views/tree/tree_node.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/resources/layout_instantiator.h"

#include "shell/resources/necro_resources_id.h"


namespace shell {

    ExampleTreePage::ExampleTreePage(ukive::Window* w)
        : Page(w) {}

    ukive::View* ExampleTreePage::onCreate(ukive::LayoutView* parent) {
        auto v = ukive::LayoutInstantiator::from(
            parent->getContext(), parent, Res::Layout::example_tree_page_layout_xml);

        // ListView
        layouter_ = new ukive::LinearListLayouter();
        tree_source_ = new ukive::TreeSource(layouter_);
        for (int i = 0; i < 10; ++i) {
            std::u16string str(u"test");
            str.append(utl::to_u16string(i));
            tree_source_->addNodeByVP(-1, str);
        }

        auto list_view = findViewById<ukive::ListView>(v, Res::Id::lv_test_tree);
        list_view->setItemSelectedListener(this);
        list_view->setLayouter(layouter_);
        list_view->setSource(tree_source_);
        list_view->setSecDimUnknown(true);

        return v;
    }

    void ExampleTreePage::onItemClicked(ukive::ListView* lv, ukive::ListItem* item) {
        int pos = item->data_pos;
        auto node = tree_source_->getNode(pos);
        if (node->is_expand) {
            node->is_expand = false;
        } else {
            node->is_expand = true;
            if (node->child_count == 0) {
                tree_source_->addNodeByVP(pos, u"Label2");
            }
        }
        tree_source_->notifyDataChanged();
    }

}
