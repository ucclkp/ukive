// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "property_items.h"

#include "ukive/views/combo_box.h"
#include "ukive/views/image_view.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/views/layout_info/sequence_layout_info.h"
#include "ukive/views/text_view.h"
#include "ukive/views/tree/tree_node_button.h"


namespace vevah {

    PropTreeLabelItem::PropTreeLabelItem(ukive::Context c)
        : ukive::ListItem(nullptr)
    {
        using namespace ukive;
        using SLI = SequenceLayoutInfo;

        auto root = new SequenceLayout(c);
        root->setOrientation(SequenceLayout::HORIZONTAL);
        root->setLayoutSize(View::LS_FILL, View::LS_AUTO);

        expand_button = new TreeNodeButton(c);
        {
            expand_button->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            expand_button->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(expand_button);
        }

        label = new TextView(c);
        {
            label->setLayoutSize(View::LS_FILL, View::LS_AUTO);
            label->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            label->setFontFamilyName(u"Consolas");
            label->setTextSize(c.dp2pxi(13));
            label->setClickable(true);
            label->setDoubleClickable(true);
            label->setFocusable(true);
            label->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(label);
        }

        item_view = root;
    }

    PropTreeEditItem::PropTreeEditItem(ukive::Context c, ukive::Editable::EditWatcher* w)
        : ukive::ListItem(nullptr)
    {
        using namespace ukive;
        using SLI = SequenceLayoutInfo;

        auto root = new SequenceLayout(c);
        root->setOrientation(SequenceLayout::HORIZONTAL);
        root->setLayoutSize(View::LS_FILL, View::LS_AUTO);

        expand_button = new TreeNodeButton(c);
        {
            expand_button->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            expand_button->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(expand_button);
        }

        label = new TextView(c);
        {
            label->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
            label->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            label->setFontFamilyName(u"Consolas");
            label->setTextSize(c.dp2pxi(13));
            label->setClickable(true);
            label->setDoubleClickable(true);
            label->setFocusable(true);
            label->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(label);
        }

        edit = new TextView(c);
        {
            edit->setLayoutSize(View::LS_FILL, View::LS_AUTO);
            edit->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            edit->setEditable(true);
            edit->setSelectable(true);
            edit->setTextSize(c.dp2pxi(13));
            edit->setClickable(true);
            edit->setDoubleClickable(true);
            edit->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            edit->getEditable()->addEditWatcher(w);
            root->addView(edit);
        }

        item_view = root;
    }

    PropTreeComboItem::PropTreeComboItem(ukive::Context c)
        : ukive::ListItem(nullptr)
    {
        using namespace ukive;
        using SLI = SequenceLayoutInfo;

        auto root = new SequenceLayout(c);
        root->setOrientation(SequenceLayout::HORIZONTAL);
        root->setLayoutSize(View::LS_FILL, View::LS_AUTO);

        expand_button = new TreeNodeButton(c);
        {
            expand_button->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            expand_button->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(expand_button);
        }

        label = new TextView(c);
        {
            label->setLayoutSize(View::LS_FILL, View::LS_AUTO);
            label->setPadding(c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2), c.dp2pxi(2));
            label->setFontFamilyName(u"Consolas");
            label->setTextSize(c.dp2pxi(13));
            label->setClickable(true);
            label->setDoubleClickable(true);
            label->setFocusable(true);
            label->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(label);
        }

        combo = new ComboBox(c);
        {
            combo->setLayoutSize(View::LS_FILL, View::LS_AUTO);
            combo->setExtraLayoutInfo(SLI::ofVertAlign(SLI::Align::CENTER));
            root->addView(combo);
        }

        item_view = root;
    }

}