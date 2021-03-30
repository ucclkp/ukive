// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_KEY_LISTENER_H_
#define UKIVE_TEXT_TEXT_KEY_LISTENER_H_

#include <string>


namespace ukive {

    class Editable;

    class TextKeyListener {
    public:
        TextKeyListener();
        ~TextKeyListener();

        void onChars(
            Editable* editable, bool can_edit, bool can_select, const std::u16string& chars);
        void onKeyDown(
            Editable* editable, bool can_edit, bool can_select, int key);
        void onKeyUp(
            Editable* editable, bool can_edit, bool can_select, int key);

    private:
        void del(Editable* editable);
        void backspace(Editable* editable);
        void newline(Editable* editable);
        void tab(Editable* editable);
    };

}

#endif  // UKIVE_TEXT_TEXT_KEY_LISTENER_H_