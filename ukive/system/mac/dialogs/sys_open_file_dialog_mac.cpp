// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "sys_open_file_dialog_mac.h"

#include "ukive/window/window.h"


namespace ukive {
namespace mac {

    SysOpenFileDialogMac::SysOpenFileDialogMac() {}

    int SysOpenFileDialogMac::show(Window* parent, uint32_t flags) {
        return -1;
    }

    void SysOpenFileDialogMac::addType(
        const std::u16string_view& types,
        const std::u16string_view& desc)
    {
        exts_.push_back({ std::u16string(types), std::u16string(desc) });
    }

    void SysOpenFileDialogMac::clearTypes() {
        exts_.clear();
    }

    const std::vector<std::u16string>& SysOpenFileDialogMac::getSelectedFiles() const {
        return sel_files_;
    }

}
}
