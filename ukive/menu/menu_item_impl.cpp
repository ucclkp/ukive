// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "menu_item_impl.h"

#include "ukive/menu/menu.h"
#include "ukive/window/window.h"


namespace ukive {

    MenuItemImpl::MenuItemImpl(Context c, int menu_id, int32_t order)
        : TextView(c),
          menu_id_(menu_id),
          order_(order)
    {
          initMenuItem();
    }

    void MenuItemImpl::initMenuItem() {
        setTextSize(getContext().dp2pxi(13));
        setEditable(false);
        setSelectable(false);
        setClickable(true);
        setPadding(getContext().dp2pxi(16), 0, getContext().dp2pxi(16), 0);
        setVertAlignment(TextLayout::Alignment::CENTER);
        autoWrap(false);
    }

    void MenuItemImpl::setParentMenu(Menu* menu) {
        parent_menu_ = menu;
    }

    Menu* MenuItemImpl::getParentMenu() const {
        return parent_menu_;
    }

    void MenuItemImpl::setItemTitle(const std::u16string_view& title) {
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

    void MenuItemImpl::setItemCheckable(bool checkable) {
        if (is_checkable_ == checkable) return;

        is_checkable_ = checkable;
        if (!checkable) {
            setItemChecked(false);
        }
        if (parent_menu_) {
            parent_menu_->notifyItemCheckable(menu_id_, checkable);
        }
    }

    void MenuItemImpl::setItemChecked(bool checked) {
        if (!is_checkable_ || is_checked_ == checked) return;

        is_checked_ = checked;
        if (parent_menu_) {
            parent_menu_->notifyItemChecked(menu_id_, checked);
        }
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

    bool MenuItemImpl::isItemCheckable() const {
        return is_checkable_;
    }

    bool MenuItemImpl::isItemChecked() const {
        return is_checked_;
    }

}