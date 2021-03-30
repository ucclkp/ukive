// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_TEXT_TEXT_WINDOW_H_
#define SHELL_TEXT_TEXT_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"
#include "ukive/menu/context_menu_callback.h"


namespace ukive {
    class ContextMenu;
}

namespace shell {

    enum {
        ID_TV_EDITOR = 0x1,
        ID_LAYOUT_ROOT = 0x2,
        ID_TOOLBAR = 0x3,
        ID_TOOLBAR_ITEM_FONT = 0x10,
        ID_TOOLBAR_ITEM_FORMAT = 0x11,
    };

    class TextWindow :
        public ukive::Window,
        public ukive::OnClickListener,
        public ukive::ContextMenuCallback
    {
    public:
        TextWindow();
        ~TextWindow();

    protected:
        // ukive::Window
        void onCreated() override;
        void onMove(int x, int y) override;
        void onResize(int param, int width, int height) override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

        // ukive::ContextMenuCallback
        bool onCreateContextMenu(ukive::ContextMenu* contextMenu, ukive::Menu* menu) override;
        bool onPrepareContextMenu(ukive::ContextMenu* contextMenu, ukive::Menu* menu) override;
        bool onContextMenuItemClicked(ukive::ContextMenu* contextMenu, ukive::MenuItem* item) override;
        void onDestroyContextMenu(ukive::ContextMenu* contextMenu) override;

    private:
        ukive::View* inflateToolbar(ukive::View* parent);

        ukive::ContextMenu* context_menu_;
    };

}

#endif  // SHELL_TEXT_TEXT_WINDOW_H_