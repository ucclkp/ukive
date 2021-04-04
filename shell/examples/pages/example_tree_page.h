// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_EXAMPLES_PAGES_EXAMPLE_TREE_PAGE_H_
#define SHELL_EXAMPLES_PAGES_EXAMPLE_TREE_PAGE_H_

#include "ukive/page/page.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/tree/tree_node.h"


namespace ukive {
    class ImageView;
    class LayoutView;
    class ListView;
    class TextView;
    class TreeNodeButton;
}

namespace shell {

    class ExampleTreeItem : public ukive::ListItem {
    public:
        explicit ExampleTreeItem(ukive::View* v);

        ukive::TextView* text_label = nullptr;
        ukive::ImageView* avatar_image = nullptr;
        ukive::TreeNodeButton* expand_button_ = nullptr;
    };

    class ExampleTreeNode : public ukive::TreeNode {
    public:
        int image_rid = 0;
        std::u16string text;
        bool is_selected = false;
    };


    class ExampleTreePage :
        public ukive::Page,
        public ukive::ListSource,
        public ukive::OnClickListener
    {
    public:
        explicit ExampleTreePage(ukive::Window* w);

        // ukive::Page
        ukive::View* onCreate(ukive::LayoutView* parent) override;
        void onDestroy() override;

        // ukive::ListSource
        ukive::ListItem* onCreateListItem(
            ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
            size_t position) override;
        void onSetListItemData(
            ukive::LayoutView* parent, ukive::ListItemEventRouter* router,
            ukive::ListItem* item) override;
        size_t onGetListDataCount(ukive::LayoutView* parent) const override;

        // OnClickListener
        void onClick(ukive::View* v) override;
        void onDoubleClick(ukive::View* v) override;

        void onItemClicked(ukive::ListView* lv, ukive::ListItem* item);

    private:
        ukive::TreeNode root_node_;
        ukive::TreeNode* selected_node_ = nullptr;
        ukive::ListView* list_view_ = nullptr;
    };

}

#endif  // SHELL_EXAMPLES_PAGES_EXAMPLE_TREE_PAGE_H_