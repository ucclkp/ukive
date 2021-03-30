// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/system/dialog/open_file_dialog.h"

#include "ukive/system/dialog/win/open_file_dialog_win.h"


namespace ukive {

    // static
    OpenFileDialog* OpenFileDialog::create() {
        return new OpenFileDialogWin();
    }

}
