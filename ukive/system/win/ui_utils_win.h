// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_UI_UTILS_WIN_H_
#define UKIVE_SYSTEM_WIN_UI_UTILS_WIN_H_

#include <Windows.h>


namespace ukive {
namespace win {

    bool isActivated();

    bool isAeroEnabled();

    HMODULE SecureSysLoadLibrary(LPCWSTR name);

}
}

#endif  // UKIVE_SYSTEM_WIN_UI_UTILS_WIN_H_