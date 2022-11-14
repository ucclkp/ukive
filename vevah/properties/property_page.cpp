#include "property_page.h"

#include "utils/log.h"
#include "utils/strings/int_conv.hpp"

#include "ukive/resources/layout_parser.h"
#include "ukive/views/text_view.h"
#include "ukive/views/tree/tree_node_button.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/combo_box.h"

#include "vevah/properties/property_items.h"
#include "vevah/resources/necro_resources_id.h"


namespace {

    enum NodeId {
        NODE_ID_LABEL,
        NODE_ID_EDIT,
        NODE_ID_COMBO,
    };

}

namespace vevah {

    PropertyPage::PropertyPage() {}

    void PropertyPage::showProps(ukive::View* v) {
        sel_view_ = v;

        if (v) {
            auto cm_node = root_node_.getChildAt(0);
            auto padding_node = (PropTreeEditNode*)cm_node->getChildAt(0);
            {
                auto& p = v->getPadding();
                padding_node->text = utl::usformat(
                    u"%d, %d, %d, %d", p.start(), p.top(), p.end(), p.bottom());
            }
            auto margin_node = (PropTreeEditNode*)cm_node->getChildAt(1);
            {
                auto& m = v->getLayoutMargin();
                margin_node->text = utl::usformat(
                    u"%d, %d, %d, %d", m.start(), m.top(), m.end(), m.bottom());
            }
            notifyDataChanged();
        }
    }

    ukive::View* PropertyPage::onCreate(ukive::Context c) {
        return ukive::LayoutParser::from(
            c, nullptr, Res::Layout::property_page_layout_xml);
    }

    void PropertyPage::onCreated(ukive::View* v) {
        auto cm_node = new PropTreeLabelNode();
        cm_node->label_text = u"公共属性";
        cm_node->setNodeId(NODE_ID_LABEL);
        root_node_.addNode(cm_node);

        auto padding_node = new PropTreeEditNode();
        padding_node->label_text = u"Padding";
        padding_node->text = u"0, 0, 0, 0";
        padding_node->setNodeId(NODE_ID_EDIT);
        cm_node->addNode(padding_node);

        auto margin_node = new PropTreeEditNode();
        margin_node->label_text = u"Margin";
        margin_node->text = u"0, 0, 0, 0";
        margin_node->setNodeId(NODE_ID_EDIT);
        cm_node->addNode(margin_node);

        list_view_ = findView<ukive::ListView>(v, Res::Id::lv_prop_tree);
        list_view_->setLayouter(new ukive::LinearListLayouter());
        list_view_->setSource(this);
        list_view_->setSecDimUnknown(false);

        root_node_.setExpanded(true);
    }

    void PropertyPage::onShow(bool show) {}

    void PropertyPage::onDestroy() {}

    ukive::ListItem* PropertyPage::onCreateListItem(
        ukive::LayoutView* parent,
        ukive::ListItemEventRouter* router,
        size_t position)
    {
        auto node_id = onGetListItemId(parent, position);
        switch (node_id) {
        case NODE_ID_LABEL:
            return new PropTreeLabelItem(parent->getContext());
        case NODE_ID_EDIT:
            return new PropTreeEditItem(parent->getContext(), this);
        case NODE_ID_COMBO:
            return new PropTreeComboItem(parent->getContext());
        default:
            ubassert(false);
            break;
        }

        return nullptr;
    }

    void PropertyPage::onSetListItemData(
        ukive::LayoutView* parent,
        ukive::ListItemEventRouter* router,
        ukive::ListItem* item)
    {
        auto node = root_node_.getExpandedDescendantAt(item->data_pos);
        switch (node->getNodeId()) {
        case NODE_ID_LABEL:
        {
            auto s_node = static_cast<PropTreeLabelNode*>(node);
            auto s_item = static_cast<PropTreeLabelItem*>(item);

            s_item->expand_button->setOnClickListener(this);
            s_item->label->setOnClickListener(this);

            s_item->ex_margins.start(int(node->getLevel() * 16));
            s_item->label->setText(s_node->label_text);

            if (node->getChildCount() == 0) {
                s_item->expand_button->setStatus(ukive::TreeNodeButton::NONE);
            } else {
                s_item->expand_button->setStatus(
                    node->isExpanded() ?
                    ukive::TreeNodeButton::EXPANDED : ukive::TreeNodeButton::COLLAPSED);
            }

            if (s_node->is_selected) {
                auto b = s_item->label->getBackground();
                if (b) {
                    b->setSolidColor(ukive::Color::Blue300);
                } else {
                    s_item->label->setBackground(
                        new ukive::Element(ukive::Color::Blue300));
                }
            } else {
                auto b = s_item->label->getBackground();
                if (b) {
                    b->setSolidColor(ukive::Color::Transparent);
                }
            }
            break;
        }

        case NODE_ID_EDIT:
        {
            auto s_node = static_cast<PropTreeEditNode*>(node);
            auto s_item = static_cast<PropTreeEditItem*>(item);

            s_item->expand_button->setOnClickListener(this);
            s_item->label->setOnClickListener(this);

            s_item->ex_margins.start(int(node->getLevel() * 16));
            s_item->label->setText(s_node->label_text);
            s_item->edit->setText(s_node->text);

            if (node->getChildCount() == 0) {
                s_item->expand_button->setStatus(ukive::TreeNodeButton::NONE);
            } else {
                s_item->expand_button->setStatus(
                    node->isExpanded() ?
                    ukive::TreeNodeButton::EXPANDED : ukive::TreeNodeButton::COLLAPSED);
            }

            if (s_node->is_selected) {
                auto b = s_item->label->getBackground();
                if (b) {
                    b->setSolidColor(ukive::Color::Blue300);
                } else {
                    s_item->label->setBackground(
                        new ukive::Element(ukive::Color::Blue300));
                }
            } else {
                auto b = s_item->label->getBackground();
                if (b) {
                    b->setSolidColor(ukive::Color::Transparent);
                }
            }
            break;
        }

        case NODE_ID_COMBO:
        {
            auto s_node = static_cast<PropTreeComboNode*>(node);
            auto s_item = static_cast<PropTreeComboItem*>(item);

            s_item->expand_button->setOnClickListener(this);
            s_item->label->setOnClickListener(this);

            s_item->ex_margins.start(int(node->getLevel() * 16));
            s_item->label->setText(s_node->label_text);
            s_item->combo->clearItems();
            for (const auto& it : s_node->data) {
                s_item->combo->addItem(it);
            }

            if (node->getChildCount() == 0) {
                s_item->expand_button->setStatus(ukive::TreeNodeButton::NONE);
            } else {
                s_item->expand_button->setStatus(
                    node->isExpanded() ?
                    ukive::TreeNodeButton::EXPANDED : ukive::TreeNodeButton::COLLAPSED);
            }

            if (s_node->is_selected) {
                auto b = s_item->label->getBackground();
                if (b) {
                    b->setSolidColor(ukive::Color::Blue300);
                } else {
                    s_item->label->setBackground(
                        new ukive::Element(ukive::Color::Blue300));
                }
            } else {
                auto b = s_item->label->getBackground();
                if (b) {
                    b->setSolidColor(ukive::Color::Transparent);
                }
            }
            break;
        }
        }
    }

    size_t PropertyPage::onGetListDataCount(
        ukive::LayoutView* parent) const
    {
        return root_node_.getExpandedDescendantCount();
    }

    int PropertyPage::onGetListItemId(
        ukive::LayoutView* parent, size_t position) const
    {
        return root_node_.getExpandedDescendantAt(position)->
            getNodeId();
    }

    void PropertyPage::onClick(ukive::View* v) {
        auto item = list_view_->findItemFromView(v);
        switch (item->item_id) {
        case NODE_ID_LABEL:
        {
            auto s_item = static_cast<PropTreeLabelItem*>(item);
            if (v == s_item->expand_button) {
                auto node = root_node_.getExpandedDescendantAt(item->data_pos);
                node->setExpanded(!node->isExpanded());
                notifyDataChanged();
            } else if (v == s_item->label) {
                auto node = static_cast<PropTreeLabelNode*>(
                    root_node_.getExpandedDescendantAt(item->data_pos));
                node->is_selected = true;
                if (selected_node_ && selected_node_ != node) {
                    static_cast<PropTreeLabelNode*>(selected_node_)->is_selected = false;
                }
                selected_node_ = node;
                notifyDataChanged();
            }
            break;
        }

        case NODE_ID_EDIT:
        {
            auto s_item = static_cast<PropTreeEditItem*>(item);
            if (v == s_item->expand_button) {
                auto node = root_node_.getExpandedDescendantAt(item->data_pos);
                node->setExpanded(!node->isExpanded());
                notifyDataChanged();
            } else if (v == s_item->label) {
                auto node = static_cast<PropTreeEditNode*>(
                    root_node_.getExpandedDescendantAt(item->data_pos));
                node->is_selected = true;
                if (selected_node_ && selected_node_ != node) {
                    static_cast<PropTreeEditNode*>(selected_node_)->is_selected = false;
                }
                selected_node_ = node;
                notifyDataChanged();
            }
            break;
        }

        case NODE_ID_COMBO:
        {
            auto s_item = static_cast<PropTreeComboItem*>(item);
            if (v == s_item->expand_button) {
                auto node = root_node_.getExpandedDescendantAt(item->data_pos);
                node->setExpanded(!node->isExpanded());
                notifyDataChanged();
            } else if (v == s_item->label) {
                auto node = static_cast<PropTreeComboNode*>(
                    root_node_.getExpandedDescendantAt(item->data_pos));
                node->is_selected = true;
                if (selected_node_ && selected_node_ != node) {
                    static_cast<PropTreeComboNode*>(selected_node_)->is_selected = false;
                }
                selected_node_ = node;
                notifyDataChanged();
            }
            break;
        }
        }
    }

    void PropertyPage::onDoubleClick(ukive::View* v) {
        auto item = list_view_->findItemFromView(v);
        switch (item->item_id) {
        case NODE_ID_LABEL:
        {
            auto label_item = static_cast<PropTreeLabelItem*>(item);
            if (v == label_item->label) {
                auto node = root_node_.getExpandedDescendantAt(item->data_pos);
                node->setExpanded(!node->isExpanded());
                notifyDataChanged();
            }
            break;
        }

        case NODE_ID_EDIT:
        {
            auto label_item = static_cast<PropTreeEditItem*>(item);
            if (v == label_item->label) {
                auto node = root_node_.getExpandedDescendantAt(item->data_pos);
                node->setExpanded(!node->isExpanded());
                notifyDataChanged();
            }
            break;
        }

        case NODE_ID_COMBO:
        {
            auto label_item = static_cast<PropTreeComboItem*>(item);
            if (v == label_item->label) {
                auto node = root_node_.getExpandedDescendantAt(item->data_pos);
                node->setExpanded(!node->isExpanded());
                notifyDataChanged();
            }
            break;
        }
        }
    }

    void PropertyPage::onTextChanged(
        ukive::Editable* editable,
        const ukive::RangeChg& rc,
        ukive::Editable::Reason r)
    {
    }

}