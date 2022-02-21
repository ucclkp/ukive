// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_COMBO_BOX_H_
#define UKIVE_VIEWS_COMBO_BOX_H_

#include "utils/weak_ref_nest.hpp"

#include "ukive/menu/inner_window.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/list/list_item_event_router.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class TextView;
    class DropdownButton;
    class ComboBoxSelectedListener;

    class ComboBox :
        public LayoutView,
        public OnClickListener,
        public OnInnerWindowEventListener,
        public ListItemEventListener,
        public ListSource
    {
    public:
        explicit ComboBox(Context c);
        ComboBox(Context c, AttrsRef attrs);
        ~ComboBox();

        void addItem(const std::u16string_view& title);
        void addItem(const std::u16string_view& title, size_t index);
        void removeItem(size_t index);
        void clearItems();

        void setSelectedIndex(int index);
        void setDefaultItem(const std::u16string_view& title);
        void setDropdownMinWidth(int min_width);
        void setListener(ComboBoxSelectedListener* l);

        int getSelectedIndex() const;

    protected:
        class TextViewListItem : public ListItem {
        public:
            explicit TextViewListItem(TextView* v)
                : ListItem(v),
                  title_label(v) {}

            TextView* title_label;
        };

        // LayoutView
        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(const Rect& new_bounds, const Rect& old_bounds) override;

        // OnClickListener
        void onClick(View* v) override;

        // ListItemEventListener
        void onItemClicked(ListView* list_view, ListItem* item, View* v) override;

        // OnInnerWindowEventListener
        void onRequestDismissByTouchOutside(InnerWindow* iw) override;

        // ListSource
        ListItem* onCreateListItem(
            LayoutView* parent, ListItemEventRouter* router, size_t position) override;
        void onSetListItemData(
            LayoutView* parent, ListItemEventRouter* router, ListItem* item) override;
        size_t onGetListDataCount(LayoutView* parent) const override;

    private:
        void initViews();
        void determineViewsSize(const SizeInfo& info);

        void show(int x, int y, int width);
        void close();

        TextView* text_view_;
        DropdownButton* button_;
        ListView* list_view_ = nullptr;

        int selected_index_ = -1;
        std::vector<std::u16string> data_;
        std::u16string default_title_;

        bool is_finished_ = true;
        int min_dropdown_width_ = 0;
        ComboBoxSelectedListener* listener_ = nullptr;
        std::shared_ptr<InnerWindow> inner_window_;
        utl::WeakRefNest<ComboBox> weak_ref_nest_;
    };

}

#endif  // UKIVE_VIEWS_COMBO_BOX_H_