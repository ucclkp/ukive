// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_WIN10_VERSION_H_
#define UKIVE_SYSTEM_WIN_WIN10_VERSION_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


namespace ukive {
namespace win {

    bool isWin10Ver(DWORD build, BYTE condition);

    // Windows 10 14393 or greater
    bool isWin10Ver1607OrGreater();

    // Windows 10 15063 or greater
    bool isWin10Ver1703OrGreater();

    // Windows 10 16299 or greater
    bool isWin10Ver1709OrGreater();

    // Windows 10 17134 or greater
    bool isWin10Ver1803OrGreater();

    // Windows 10 17763 or greater
    bool isWin10Ver1809OrGreater();

}
}

#endif  // UKIVE_SYSTEM_WIN_WIN10_VERSION_H_