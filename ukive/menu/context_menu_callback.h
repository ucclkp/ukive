// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MENU_CONTEXT_MENU_CALLBACK_H_
#define UKIVE_MENU_CONTEXT_MENU_CALLBACK_H_


namespace ukive {

    class Menu;
    class MenuItem;
    class ContextMenu;

    class ContextMenuCallback {
    public:
        virtual ~ContextMenuCallback() = default;

        virtual bool onCreateContextMenu(ContextMenu *contextMenu, Menu *menu) = 0;
        virtual bool onPrepareContextMenu(ContextMenu *contextMenu, Menu *menu) = 0;
        virtual bool onContextMenuItemClicked(ContextMenu *contextMenu, MenuItem *item) = 0;
        virtual void onDestroyContextMenu(ContextMenu *contextMenu) = 0;
    };

}

#endif  // UKIVE_MENU_CONTEXT_MENU_CALLBACK_H_