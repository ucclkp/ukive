// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_ACTION_MENU_CALLBACK_H_
#define UKIVE_TEXT_TEXT_ACTION_MENU_CALLBACK_H_


namespace ukive {

    class Menu;
    class MenuItem;
    class TextActionMenu;

    class TextActionMenuCallback {
    public:
        virtual ~TextActionMenuCallback() = default;

        virtual bool onCreateActionMode(TextActionMenu* mode, Menu* menu) = 0;
        virtual bool onPrepareActionMode(TextActionMenu* mode, Menu* menu) = 0;
        virtual bool onActionItemClicked(TextActionMenu* mode, MenuItem* item) = 0;
        virtual void onDestroyActionMode(TextActionMenu* mode) = 0;

        // 获取相对于当前窗口的坐标
        virtual void onGetContentPosition(int* x, int* y) = 0;
    };

}

#endif  // UKIVE_TEXT_TEXT_ACTION_MENU_CALLBACK_H_