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
          item_height_(LS_AUTO),
          callback_(nullptr)
    {
          initMenu();
    }


    void MenuImpl::initMenu() {}

    void MenuImpl::setMenuItemHeight(int height) {
        item_height_ = height;
    }

    void MenuImpl::setCallback(MenuCallback* callback) {
        callback_ = callback;
    }

    MenuCallback* MenuImpl::getCallback() const {
        return callback_;
    }

    MenuItem* MenuImpl::addItem(int id, int order, const std::u16string& title) {
        auto item = new MenuItemImpl(getContext(), id, order);
        item->setItemTitle(title);

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

        view->setLayoutSize(LS_FILL, item_height_);

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

    void MenuImpl::onClick(View* v) {
        callback_->onMenuItemClicked(this, static_cast<MenuItemImpl*>(v));
    }

}