// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_CLIPBOARD_MANAGER_H_
#define UKIVE_SYSTEM_CLIPBOARD_MANAGER_H_

#include <string>


namespace ukive {

    class ClipboardManager {
    public:
        ClipboardManager();
        ~ClipboardManager();

        static void saveToClipboard(const std::u16string_view& text);
        static std::u16string getFromClipboard();
    };

}

#endif // !UKIVE_SYSTEM_CLIPBOARD_MANAGER_H_