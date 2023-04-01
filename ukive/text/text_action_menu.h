// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_ACTION_MENU_H_
#define UKIVE_TEXT_TEXT_ACTION_MENU_H_

#include <memory>

#include "ukive/basics/levitator.h"
#include "ukive/menu/menu_callback.h"


namespace ukive {

    class Menu;
    class MenuImpl;
    class MenuItem;
    class TextActionMenuCallback;

    class TextActionMenu : public MenuCallback {
    public:
        TextActionMenu(Window* w, TextActionMenuCallback* callback);
        ~TextActionMenu();

        void onCreateMenu(Menu* menu) override;
        void onPrepareMenu(Menu* menu) override;
        bool onMenuItemClicked(Menu* menu, MenuItem* item) override;

        Menu* getMenu() const;

        void invalidateMenu();
        void invalidatePosition();

        void show();
        void close();

        void setId(int id);
        int getId() const;

    private:
        int id_ = -1;
        int menu_width_;
        int menu_item_height_;
        bool is_finished_;

        Window* window_;
        TextActionMenuCallback* callback_;

        MenuImpl* menu_impl_;
        std::shared_ptr<Levitator> levitator_;
    };

}

#endif  // UKIVE_TEXT_TEXT_ACTION_MENU_H_