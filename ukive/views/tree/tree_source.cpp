// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "tree_source.h"

#include "utils/log.h"

#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/image_view.h"
#include "ukive/views/text_view.h"
#include "ukive/views/tree/tree_node_button.h"
#include "ukive/views/tree/tree_node.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/elements/color_element.h"
#include "ukive/graphics/color.h"
#include "ukive/window/window.h"


namespace ukive {

    // TreeItem
    TreeItem::TreeItem(View* v)
        : ListItem(v) {}

    // TreeSource
    TreeSource::TreeSource(LinearListLayouter* layouter)
        : layouter_(layouter) {}

    ListItem* TreeSource::onListCreateItem(
        LayoutView* parent, int position)
    {
        auto c = parent->getContext();

        auto layout = new RestraintLayout(c);
        layout->setLayoutSize(View::LS_AUTO, View::LS_AUTO);

        using Rlp = RestraintLayoutInfo;

        auto expand_btn = new TreeNodeButton(c);
        expand_btn->setPadding(c.dp2px(2), c.dp2px(2), c.dp2px(2), c.dp2px(2));
        expand_btn->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
        auto eb_lp = Rlp::Builder()
            .start(layout->getId())
            .top(layout->getId())
            .bottom(layout->getId())
            .build();
        expand_btn->setExtraLayoutInfo(eb_lp);
        layout->addView(expand_btn);

        auto avatar_view = new ImageView(c);
        avatar_view->setImage({});
        avatar_view->setLayoutSize(c.dp2px(12), c.dp2px(12));
        avatar_view->setLayoutMargin(c.dp2px(2), c.dp2px(2), 0, c.dp2px(2));
        Rlp* av_lp = new Rlp();
        av_lp->startHandle(expand_btn->getId(), Rlp::END);
        av_lp->topHandle(layout->getId(), Rlp::TOP);
        av_lp->bottomHandle(layout->getId(), Rlp::BOTTOM);
        avatar_view->setExtraLayoutInfo(av_lp);
        layout->addView(avatar_view);

        auto title_label = new TextView(c);
        title_label->setFontFamilyName(u"Consolas");
        title_label->setTextSize(c.dp2px(13));
        title_label->setClickable(true);
        title_label->setDoubleClickable(true);
        title_label->setPadding(c.dp2px(2), c.dp2px(2), c.dp2px(2), c.dp2px(2));
        title_label->setLayoutSize(View::LS_FILL, View::LS_AUTO);
        Rlp* tl_lp = new Rlp();
        tl_lp->startHandle(avatar_view->getId(), Rlp::END);
        tl_lp->endHandle(layout->getId(), Rlp::END);
        tl_lp->topHandle(layout->getId(), Rlp::TOP);
        tl_lp->bottomHandle(layout->getId(), Rlp::BOTTOM);
        title_label->setExtraLayoutInfo(tl_lp);
        layout->addView(title_label);

        auto root = new TreeItem(layout);
        root->avatar_image = avatar_view;
        root->text_label = title_label;
        root->expand_button_ = expand_btn;

        return root;
    }

    void TreeSource::onListSetItemData(ListItem* item, int position) {
        auto data = data_list_[getRealPos(position)].get();
        auto tree_item = static_cast<TreeItem*>(item);

        tree_item->expand_button_->setOnClickListener(this);
        tree_item->text_label->setOnClickListener(this);

        tree_item->ex_margins.left = data->level * 16;
        tree_item->text_label->setText(data->text);

        if (data->child_count == 0) {
            tree_item->expand_button_->setStatus(TreeNodeButton::NONE);
        } else {
            tree_item->expand_button_->setStatus(
                data->is_expand ? TreeNodeButton::EXPANDED : TreeNodeButton::COLLAPSED);
        }

        if (data->is_selected) {
            auto b = tree_item->text_label->getBackground();
            if (b) {
                static_cast<ColorElement*>(b)->setColor(Color::Blue300);
            } else {
                tree_item->text_label->setBackground(new ColorElement(Color::Blue300));
            }
        } else {
            auto b = tree_item->text_label->getBackground();
            if (b) {
                static_cast<ColorElement*>(b)->setColor(Color::Transparent);
            }
        }

        //LOG(INFO) << "ListSource::onListSetItemData():" << position << " data has been bound.";
    }

    int TreeSource::onListGetDataCount() {
        int count = 0;
        for (size_t i = 0; i < data_list_.size(); ++i, ++count) {
            auto dat = data_list_[i].get();
            if (!dat->is_expand) {
                i += dat->child_count;
            }
        }
        return count;
    }

    void TreeSource::onClick(View* v) {
        auto item = static_cast<TreeItem*>(
            layouter_->findItemFromView(v->getParent()));
        if (v == item->expand_button_) {
            int pos = item->data_pos;
            auto node = getNode(pos);
            if (node->is_expand) {
                node->is_expand = false;
            } else {
                node->is_expand = true;
            }
            notifyDataChanged();
        } else if (v == item->text_label) {
            int pos = item->data_pos;
            auto real_pos = getRealPos(pos);
            data_list_[real_pos]->is_selected = true;
            if (selected_rpos_ != -1 && selected_rpos_!= real_pos) {
                data_list_[selected_rpos_]->is_selected = false;
            }
            selected_rpos_ = real_pos;
            notifyDataChanged();
        }
    }

    void TreeSource::onDoubleClick(View* v) {
        auto item = static_cast<TreeItem*>(
            layouter_->findItemFromView(v->getParent()));
        if (v == item->text_label) {
            int pos = item->data_pos;
            auto node = getNode(pos);
            if (node->is_expand) {
                node->is_expand = false;
            } else {
                node->is_expand = true;
            }
            notifyDataChanged();
        }
    }

    int TreeSource::addNodeByVP(int parent_vpos, const std::u16string& text) {
        return addNodeByRP(getRealPos(parent_vpos), text);
    }

    int TreeSource::addNodeByRP(int parent_rpos, const std::u16string& text) {
        auto data = std::make_shared<TreeNode>();
        data->image_rid = 0;
        data->text = text;

        size_t off;
        if (parent_rpos >= 0) {
            auto parent = data_list_[parent_rpos].get();

            data->level = parent->level + 1;
            data->parent = parent;

            off = parent_rpos + parent->child_count + 1;
            parent->addChildCount(1);
        } else {
            data->level = 0;
            data->parent = nullptr;
            off = data_list_.size();
        }

        data_list_.insert(data_list_.begin() + off, data);
        notifyDataChanged();

        return off;
    }

    TreeNode* TreeSource::getNode(int vpos) {
        return data_list_[getRealPos(vpos)].get();
    }

    void TreeSource::modifyNode(int vpos, const std::u16string& text) {
        auto real_pos = getRealPos(vpos);
        auto& dat = data_list_[real_pos];
        dat->text = text;

        notifyDataChanged();
    }

    void TreeSource::removeNode(int vpos) {
        auto real_pos = getRealPos(vpos);
        auto& dat = data_list_[real_pos];
        data_list_.erase(
            data_list_.begin() + real_pos,
            data_list_.begin() + real_pos + dat->child_count);

        if (selected_rpos_ >= real_pos && selected_rpos_ < real_pos + dat->child_count) {
            selected_rpos_ = -1;
        }
        notifyDataChanged();
    }

    void TreeSource::clearNodes() {
        data_list_.clear();
        notifyDataChanged();
    }

    int TreeSource::getRealPos(int vpos) const {
        for (size_t i = 0; i < data_list_.size(); ++i, --vpos) {
            if (vpos == 0) {
                return i;
            }
            auto dat = data_list_[i].get();
            if (!dat->is_expand) {
                i += dat->child_count;
            }
        }
        return -1;
    }

}
