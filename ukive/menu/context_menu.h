// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_CONTEXT_MENU_H_
#define UKIVE_MENU_CONTEXT_MENU_H_

#include <memory>

#include "ukive/basics/levitator.h"
#include "ukive/menu/menu_callback.h"


namespace ukive {

    class Menu;
    class MenuImpl;
    class MenuItem;
    class ContextMenuCallback;

    class ContextMenu : public MenuCallback, public OnLeviatorEventListener {
    public:
        ContextMenu(Window* w, ContextMenuCallback* callback);
        ~ContextMenu();

        void onCreateMenu(Menu* menu) override;
        void onPrepareMenu(Menu* menu) override;
        bool onMenuItemClicked(Menu* menu, MenuItem* item) override;

        Menu* getMenu() const;

        void invalidateMenu();

        void show(View* rel, int x, int y);
        void show(View* anchor, int gravity);
        void close();

        void setId(int id);
        int getId() const;

    protected:
        // OnLeviatorEventListener
        void onRequestDismissByTouchOutside(Levitator* lev) override;

    private:
        int id_ = -1;
        int menu_width_;
        int menu_item_height_;
        bool is_finished_;

        ContextMenuCallback* callback_;

        Window* window_;
        MenuImpl* menu_;
        std::shared_ptr<Levitator> levitator_;
    };

}

#endif  // UKIVE_MENU_CONTEXT_MENU_H_