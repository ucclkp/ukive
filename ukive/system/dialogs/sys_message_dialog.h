// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_DIALOGS_SYS_MESSAGE_DIALOG_H_
#define UKIVE_SYSTEM_DIALOGS_SYS_MESSAGE_DIALOG_H_

#include <string>


namespace ukive {

    class Window;

    class SysMessageDialog {
    public:
        static int show(
            Window* parent,
            const std::u16string_view& title,
            const std::u16string_view& text,
            int flags);
    };

}

#endif  // UKIVE_SYSTEM_DIALOGS_SYS_MESSAGE_DIALOG_H_