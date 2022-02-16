// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_MENU_ITEM_H_
#define UKIVE_MENU_MENU_ITEM_H_

#include <string>


namespace ukive {

    class MenuItem {
    public:
        virtual ~MenuItem() = default;

        virtual void setItemTitle(const std::u16string_view& title) = 0;
        virtual void setItemVisible(bool visible) = 0;
        virtual void setItemEnabled(bool enable) = 0;

        virtual int getItemId() const = 0;
        virtual int32_t getItemOrder() const = 0;
        virtual const std::u16string& getItemTitle() const = 0;
        virtual bool isItemVisible() const = 0;
        virtual bool isItemEnabled() const = 0;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_H_