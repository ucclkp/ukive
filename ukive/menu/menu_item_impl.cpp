// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "menu_item_impl.h"

#include "ukive/window/window.h"


namespace ukive {

    MenuItemImpl::MenuItemImpl(Context c, int menu_id, int32_t order)
        : TextView(c),
          menu_id_(menu_id),
          is_visible_(true),
          order_(order)
    {
          initMenuItem();
    }


    void MenuItemImpl::initMenuItem() {
        setTextSize(getContext().dp2pxi(13));
        setIsEditable(false);
        setIsSelectable(false);
        setClickable(true);
        setPadding(getContext().dp2pxi(16), 0, getContext().dp2pxi(16), 0);
        setParagraphAlignment(TextLayout::Alignment::CENTER);
        autoWrap(false);
    }


    void MenuItemImpl::setItemTitle(const std::u16string& title) {
        setText(title);
    }

    void MenuItemImpl::setItemVisible(bool visible) {
        if (is_visible_ == visible) return;

        is_visible_ = visible;
        setVisibility(visible ? SHOW : VANISHED);
    }

    void MenuItemImpl::setItemEnabled(bool enable) {
        setEnabled(enable);
    }

    int MenuItemImpl::getItemId() const {
        return menu_id_;
    }

    int32_t MenuItemImpl::getItemOrder() const {
        return order_;
    }

    const std::u16string& MenuItemImpl::getItemTitle() const {
        return getText();
    }

    bool MenuItemImpl::isItemVisible() const {
        return is_visible_;
    }

    bool MenuItemImpl::isItemEnabled() const {
        return isEnabled();
    }

}