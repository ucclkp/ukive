// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_MENU_IMPL_H_
#define UKIVE_MENU_MENU_IMPL_H_

#include "ukive/menu/menu.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class View;
    class MenuCallback;

    class MenuImpl : public SequenceLayout, public Menu, public OnClickListener {
    public:
        explicit MenuImpl(Context c);

        void setMenuItemHeight(int height);

        void setCallback(MenuCallback* callback) override;
        MenuCallback* getCallback() const override;

        MenuItem* addItem(int id, int order, const std::u16string_view& title) override;
        bool removeItem(int id) override;
        bool hasItem(int id) const override;
        MenuItem* findItem(int id) const override;
        size_t getItemCount() const override;

        void onClick(View* v) override;

    private:
        void initMenu();

        int item_height_;
        MenuCallback* callback_;
    };

}

#endif  // UKIVE_MENU_MENU_IMPL_H_