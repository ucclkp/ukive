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

        void setItemTitle(const std::u16string_view& title) override;
        void setItemVisible(bool visible) override;
        void setItemEnabled(bool enable) override;

        int getItemId() const override;
        int32_t getItemOrder() const override;
        const std::u16string& getItemTitle() const override;
        bool isItemVisible() const override;
        bool isItemEnabled() const override;

    private:
        void initMenuItem();

        int menu_id_;
        bool is_visible_;
        int32_t order_;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_IMPL_H_