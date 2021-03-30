// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_MENU_H_
#define UKIVE_MENU_MENU_H_

#include <string>


namespace ukive {

    class MenuItem;
    class MenuCallback;

    class Menu {
    public:
        virtual ~Menu() = default;

        virtual void setCallback(MenuCallback* callback) = 0;
        virtual MenuCallback* getCallback() const = 0;

        virtual MenuItem* addItem(int id, int order, const std::u16string& title) = 0;
        virtual bool removeItem(int id) = 0;
        virtual bool hasItem(int id) const = 0;
        virtual MenuItem* findItem(int id) const = 0;
        virtual int getItemCount() const = 0;
    };

}

#endif  // UKIVE_MENU_MENU_H_