// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_MENU_CALLBACK_H_
#define UKIVE_MENU_MENU_CALLBACK_H_


namespace ukive {

    class Menu;
    class MenuItem;

    class MenuCallback {
    public:
        virtual ~MenuCallback() = default;

        virtual void onCreateMenu(Menu* menu) = 0;
        virtual void onPrepareMenu(Menu* menu) = 0;
        virtual bool onMenuItemClicked(Menu* menu, MenuItem* item) = 0;
    };

}

#endif  // UKIVE_MENU_MENU_CALLBACK_H_