// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "open_file_dialog.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/system/win/dialogs/open_file_dialog_win.h"
#endif


namespace ukive {

    // static
    OpenFileDialog* OpenFileDialog::create() {
#ifdef OS_WINDOWS
        return new win::OpenFileDialogWin();
#endif
    }

}
