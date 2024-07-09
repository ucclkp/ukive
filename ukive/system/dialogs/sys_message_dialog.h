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

    enum SysMessageDialogFlags {
        SMDF_BTN_YES = 0u,
        SMDF_BTN_YES_CANCEL = 1u,
        SMDF_BTN_YES_NO_CANCEL = 2u,
        SMDF_BTN_RETRY_CANCEL = 3u,

        SMDF_ICO_INFO = 1u << 8u,
        SMDF_ICO_WARN = 2u << 8u,
        SMDF_ICO_ERRO = 3u << 8u,
    };

    class SysMessageDialog {
    public:
        static int show(
            Window* parent,
            const std::u16string_view& title,
            const std::u16string_view& text,
            unsigned int flags);
    };

}

#endif  // UKIVE_SYSTEM_DIALOGS_SYS_MESSAGE_DIALOG_H_