// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "menu_impl.h"

#include "ukive/menu/menu_item_impl.h"
#include "ukive/elements/ripple_element.h"
#include "ukive/menu/menu_callback.h"


namespace ukive {

    MenuImpl::MenuImpl(Context c)
        : SequenceLayout(c),
          item_vert_padding_(c.dp2pxi(6)),
          callback_(nullptr)
    {
          initMenu();
    }


    void MenuImpl::initMenu() {}

    void MenuImpl::setMenuItemVertPadding(int padding) {
        item_vert_padding_ = padding;
    }

    void MenuImpl::setCallback(MenuCallback* callback) {
        callback_ = callback;
    }

    MenuCallback* MenuImpl::getCallback() const {
        return callback_;
    }

    MenuItem* MenuImpl::addItem(
        int id, int order,
        const std::u16string_view& title,
        bool checkable)
    {
        auto item = new MenuItemImpl(getContext(), id, order);
        item->setParentMenu(this);
        item->setItemCheckable(checkable);
        item->setItemTitle(title);
        auto padding = item->getPadding();
        padding.top(item_vert_padding_);
        padding.bottom(item_vert_padding_);
        item->setPadding(padding);

        View* view = item;
        view->setBackground(new RippleElement());
        view->setOnClickListener(this);

        size_t insertedIndex = getChildCount();
        for (size_t i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            MenuItem* childItem = static_cast<MenuItemImpl*>(child);
            if (childItem->getItemOrder() > order) {
                insertedIndex = i;
                break;
            }
        }

        view->setLayoutSize(LS_FILL, LS_AUTO);

        addView(insertedIndex, view);
        return item;
    }

    bool MenuImpl::removeItem(int id) {
        bool removed = false;

        for (size_t i = 0; i < getChildCount();) {
            View* child = getChildAt(i);
            auto item = static_cast<MenuItemImpl*>(child);
            if (item && item->getItemId() == id) {
                removeView(child);
                removed = true;
            } else {
                ++i;
            }
        }

        return removed;
    }

    bool MenuImpl::hasItem(int id) const {
        for (const auto child : *this) {
            MenuItem* item = static_cast<MenuItemImpl*>(child);
            if (item && item->getItemId() == id) {
                return true;
            }
        }

        return false;
    }

    MenuItem* MenuImpl::findItem(int id) const {
        for (const auto child : *this) {
            MenuItem* item = static_cast<MenuItemImpl*>(child);
            if (item && item->getItemId() == id) {
                return item;
            }
        }

        return nullptr;
    }

    size_t MenuImpl::getItemCount() const {
        return getChildCount();
    }

    void MenuImpl::notifyItemCheckable(int id, bool checkable) {

    }

    void MenuImpl::notifyItemChecked(int id, bool checked) {

    }

    void MenuImpl::onClick(View* v) {
        callback_->onMenuItemClicked(this, static_cast<MenuItemImpl*>(v));
    }

}