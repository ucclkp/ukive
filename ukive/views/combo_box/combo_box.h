// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_COMBO_BOX_COMBO_BOX_H_
#define UKIVE_VIEWS_COMBO_BOX_COMBO_BOX_H_

#include "utils/weak_ref_nest.hpp"

#include "ukive/menu/inner_window.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/list/list_view.h"


namespace ukive {

    class TextView;
    class DropdownButton;
    class ComboListSource;

    class ComboBox :
        public LayoutView,
        public OnClickListener,
        public OnInnerWindowEventListener,
        public ListItemSelectedListener {
    public:
        explicit ComboBox(Context c);
        ComboBox(Context c, AttrsRef attrs);
        ~ComboBox();

        void addItem(const std::u16string& title);

    protected:
        // LayoutView
        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(const Rect& new_bounds, const Rect& old_bounds) override;

        // OnClickListener
        void onClick(View* v) override;

        // OnInnerWindowEventListener
        void onRequestDismissByTouchOutside(InnerWindow* iw) override;

        // ListItemSelectedListener
        void onItemClicked(ListView* lv, ListItem* item) override;

    private:
        void initViews();
        void determineViewsSize(const SizeInfo& info);

        void show(int x, int y, int width);
        void close();

        TextView* text_view_;
        DropdownButton* button_;

        bool is_finished_ = true;
        int min_dropdown_width_ = 0;
        ListView* list_view_ = nullptr;
        ComboListSource* source_ = nullptr;
        std::shared_ptr<InnerWindow> inner_window_;
        utl::WeakRefNest<ComboBox> weak_ref_nest_;
    };

}

#endif  // UKIVE_VIEWS_COMBO_BOX_COMBO_BOX_H_