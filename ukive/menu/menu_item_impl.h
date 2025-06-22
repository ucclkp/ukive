// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_MENU_ITEM_IMPL_H_
#define UKIVE_MENU_MENU_ITEM_IMPL_H_

#include "ukive/menu/menu_item.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class Window;

    class MenuItemImpl : public TextView, public MenuItem {
    public:
        MenuItemImpl(Context c, int menu_id, int32_t order);

        void setParentMenu(Menu* menu) override;
        Menu* getParentMenu() const override;

        void setItemTitle(const std::u16string_view& title) override;
        void setItemVisible(bool visible) override;
        void setItemEnabled(bool enable) override;
        void setItemCheckable(bool checkable) override;
        void setItemChecked(bool checked) override;

        int getItemId() const override;
        int32_t getItemOrder() const override;
        const std::u16string& getItemTitle() const override;
        bool isItemVisible() const override;
        bool isItemEnabled() const override;
        bool isItemCheckable() const override;
        bool isItemChecked() const override;

    private:
        void initMenuItem();

        Menu* parent_menu_ = nullptr;
        int menu_id_;
        int32_t order_;
        bool is_visible_ = true;
        bool is_checkable_ = false;
        bool is_checked_ = false;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_IMPL_H_