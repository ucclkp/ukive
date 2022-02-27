// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_MAC_DIALOGS_SYS_OPEN_FILE_DIALOG_MAC_H_
#define UKIVE_SYSTEM_MAC_DIALOGS_SYS_OPEN_FILE_DIALOG_MAC_H_

#include "ukive/system/dialogs/sys_open_file_dialog.h"


namespace ukive {
namespace mac {

    class SysOpenFileDialogMac : public OpenFileDialog {
    public:
        SysOpenFileDialogMac();

        int show(Window* parent, uint32_t flags) override;

        void addType(
            const std::u16string_view& types,
            const std::u16string_view& desc) override;
        void clearTypes() override;

        const std::vector<std::u16string>& getSelectedFiles() const override;

    private:
        std::vector<std::u16string> sel_files_;
        std::vector<std::pair<std::u16string, std::u16string>> exts_;
    };

}
}

#endif  // UKIVE_SYSTEM_MAC_DIALOGS_SYS_OPEN_FILE_DIALOG_MAC_H_
