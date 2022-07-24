// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_PROPERTIES_PROPERTY_ITEMS_H_
#define VEVAH_PROPERTIES_PROPERTY_ITEMS_H_

#include <string>
#include <vector>

#include "ukive/text/editable.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/tree/tree_node.h"
#include "ukive/window/context.h"


namespace ukive {
    class ComboBox;
    class ImageView;
    class LayoutView;
    class ListView;
    class TextView;
    class TreeNodeButton;
}

namespace vevah {

    class PropTreeLabelItem : public ukive::ListItem {
    public:
        explicit PropTreeLabelItem(ukive::Context c);

        ukive::TextView* label = nullptr;
        ukive::TreeNodeButton* expand_button = nullptr;
    };

    class PropTreeEditItem : public ukive::ListItem {
    public:
        explicit PropTreeEditItem(ukive::Context c, ukive::Editable::EditWatcher* w);

        ukive::TextView* label = nullptr;
        ukive::TextView* edit = nullptr;
        ukive::TreeNodeButton* expand_button = nullptr;
    };

    class PropTreeComboItem : public ukive::ListItem {
    public:
        explicit PropTreeComboItem(ukive::Context c);

        ukive::TextView* label = nullptr;
        ukive::ComboBox* combo = nullptr;
        ukive::TreeNodeButton* expand_button = nullptr;
    };


    class PropTreeLabelNode : public ukive::TreeNode {
    public:
        std::u16string label_text;
        bool is_selected = false;
    };

    class PropTreeEditNode : public ukive::TreeNode {
    public:
        std::u16string label_text;
        std::u16string text;
        bool is_selected = false;
    };

    class PropTreeComboNode : public ukive::TreeNode {
    public:
        std::u16string label_text;
        std::vector<std::u16string> data;
        bool is_selected = false;
    };

}

#endif  // VEVAH_PROPERTIES_PROPERTY_ITEMS_H_