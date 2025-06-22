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

        MenuItem* addItem(int id, int order, const std::u16string_view& title) {
            return addItem(id, order, title, false);
        }
        virtual MenuItem* addItem(int id, int order, const std::u16string_view& title, bool checkable) = 0;
        virtual bool removeItem(int id) = 0;
        virtual bool hasItem(int id) const = 0;
        virtual MenuItem* findItem(int id) const = 0;
        virtual size_t getItemCount() const = 0;

        virtual void notifyItemCheckable(int id, bool checkable) = 0;
        virtual void notifyItemChecked(int id, bool checked) = 0;
    };

}

#endif  // UKIVE_MENU_MENU_H_