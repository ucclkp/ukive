// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_PROPERTIES_PROPERTY_PAGE_H_
#define VEVAH_PROPERTIES_PROPERTY_PAGE_H_

#include "ukive/page/page.h"
#include "ukive/text/editable.h"
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

namespace vevah {

    class PropertyPage :
        public ukive::Page,
        public ukive::ListSource,
        public ukive::OnClickListener,
        public ukive::Editable::EditWatcher
    {
    public:
        PropertyPage();

        void showProps(ukive::View* v);

        // ukive::Page
        ukive::View* onCreate(ukive::Context c) override;
        void onCreated(ukive::View* v) override;
        void onShow(bool show) override;
        void onDestroy() override;

        // ukive::ListSource
        ukive::ListItem* onCreateListItem(
            ukive::LayoutView* parent,
            ukive::ListItemEventRouter* router,
            size_t position) override;
        void onSetListItemData(
            ukive::LayoutView* parent,
            ukive::ListItemEventRouter* router,
            ukive::ListItem* item) override;
        size_t onGetListDataCount(
            ukive::LayoutView* parent) const override;
        int onGetListItemId(
            ukive::LayoutView* parent, size_t position) const override;

        // OnClickListener
        void onClick(ukive::View* v) override;
        void onDoubleClick(ukive::View* v) override;

        // ukive::Editable::EditWatcher
        void onTextChanged(
            ukive::Editable* editable,
            const ukive::RangeChg& rc,
            ukive::Editable::Reason r) override;

    private:
        ukive::View* sel_view_ = nullptr;

        ukive::TreeNode root_node_;
        ukive::TreeNode* selected_node_ = nullptr;
        ukive::ListView* list_view_ = nullptr;
    };

}

#endif  // VEVAH_PROPERTIES_PROPERTY_PAGE_H_