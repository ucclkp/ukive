// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TREE_TREE_SOURCE_H_
#define UKIVE_VIEWS_TREE_TREE_SOURCE_H_

#include <vector>

#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_source.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class TextView;
    class TreeNode;
    class ImageView;
    class TreeNodeButton;
    class LayoutView;
    class LinearListLayouter;

    class TreeItem : public ListItem {
    public:
        explicit TreeItem(View* v);

        TextView* text_label = nullptr;
        ImageView* avatar_image = nullptr;
        TreeNodeButton* expand_button_ = nullptr;
    };


    class TreeSource : public ListSource, public OnClickListener {
    public:
        explicit TreeSource(LinearListLayouter* layouter);

        ListItem* onListCreateItem(LayoutView* parent, int position) override;
        void onListSetItemData(ListItem* item, int position) override;
        int onListGetDataCount() override;

        // OnClickListener
        void onClick(View* v) override;
        void onDoubleClick(View* v) override;

        int addNodeByVP(int parent_vpos, const std::u16string& text);
        int addNodeByRP(int parent_rpos, const std::u16string& text);
        TreeNode* getNode(int vpos);
        void modifyNode(int vpos, const std::u16string& text);
        void removeNode(int vpos);
        void clearNodes();

    private:
        int getRealPos(int vpos) const;

        int selected_rpos_ = -1;
        LinearListLayouter* layouter_;
        std::vector<std::shared_ptr<TreeNode>> data_list_;
    };

}

#endif  // UKIVE_VIEWS_TREE_TREE_SOURCE_H_