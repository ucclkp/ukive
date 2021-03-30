// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/combo_box/combo_list_source.h"

#include "utils/convert.h"
#include "utils/number.hpp"

#include "ukive/elements/ripple_element.h"
#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/text_view.h"
#include "ukive/window/window.h"


namespace ukive {

    ComboListSource::ComboListSource() {}

    ListItem* ComboListSource::onListCreateItem(LayoutView* parent, int position) {
        auto c = parent->getContext();

        auto title_tv = new TextView(c);
        title_tv->setPadding(c.dp2pxi(16), c.dp2pxi(8), c.dp2pxi(16), c.dp2pxi(8));
        title_tv->setClickable(true);
        title_tv->setBackground(new RippleElement());
        title_tv->setLayoutSize(View::LS_FILL, View::LS_AUTO);

        return new TextViewListItem(title_tv);
    }

    void ComboListSource::onListSetItemData(ListItem* item, int position) {
        auto& data = data_.at(position);
        auto combo_item = reinterpret_cast<TextViewListItem*>(item);
        combo_item->title_label->setText(data);
    }

    int ComboListSource::onListGetDataCount() {
        return utl::num_cast<int>(data_.size());
    }

    void ComboListSource::addItem(const std::u16string& title) {
        data_.push_back(title);
        notifyDataChanged();
    }

    void ComboListSource::addItem(int pos, const std::u16string& title) {
        if (pos < 0 || pos >= utl::num_cast<int>(data_.size())) {
            return;
        }

        data_.insert(data_.begin() + pos, title);
        notifyDataChanged();
    }

    void ComboListSource::modifyItem(int pos, const std::u16string& title) {
        if (pos < 0 || pos >= utl::num_cast<int>(data_.size())) {
            return;
        }
        data_[pos] = title;
        notifyDataChanged();
    }

    void ComboListSource::removeItem(int pos) {
        if (pos < 0 || pos >= utl::num_cast<int>(data_.size())) {
            return;
        }

        data_.erase(data_.begin() + pos);
        notifyDataChanged();
    }

    std::u16string ComboListSource::getItemData(int pos) const {
        return data_[pos];
    }

}