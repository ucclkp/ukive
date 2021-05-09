// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/system/win/win10_version.h"

#define WIN10_1511_BUILD  10586
#define WIN10_1607_BUILD  14393
#define WIN10_1703_BUILD  15063
#define WIN10_1709_BUILD  16299
#define WIN10_1803_BUILD  17134
#define WIN10_1809_BUILD  17763
#define WIN10_1903_BUILD  18362


namespace ukive {
namespace win {

    bool isWin10Ver(DWORD build, BYTE condition) {
        OSVERSIONINFOEXW osi;
        osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
        osi.dwBuildNumber = build;
        osi.dwMajorVersion = HIBYTE(_WIN32_WINNT_WIN10);
        osi.dwMinorVersion = LOBYTE(_WIN32_WINNT_WIN10);
        osi.wServicePackMajor = 0;
        osi.wServicePackMinor = 0;

        auto condition_mask = VerSetConditionMask(
            VerSetConditionMask(
                VerSetConditionMask(
                    VerSetConditionMask(
                        VerSetConditionMask(0, VER_BUILDNUMBER, condition),
                        VER_MAJORVERSION, condition),
                    VER_MINORVERSION, condition),
                VER_SERVICEPACKMAJOR, condition),
            VER_SERVICEPACKMINOR, condition);

        BOOL result = VerifyVersionInfoW(
            &osi,
            VER_BUILDNUMBER |
            VER_MAJORVERSION |
            VER_MINORVERSION |
            VER_SERVICEPACKMAJOR |
            VER_SERVICEPACKMINOR,
            condition_mask);

        return (result != FALSE);
    }

    bool isWin10Ver1511OrGreater() {
        return isWin10Ver(WIN10_1511_BUILD, VER_GREATER_EQUAL);
    }

    bool isWin10Ver1607OrGreater() {
        return isWin10Ver(WIN10_1607_BUILD, VER_GREATER_EQUAL);
    }

    bool isWin10Ver1703OrGreater() {
        return isWin10Ver(WIN10_1703_BUILD, VER_GREATER_EQUAL);
    }

    bool isWin10Ver1709OrGreater() {
        return isWin10Ver(WIN10_1709_BUILD, VER_GREATER_EQUAL);
    }

    bool isWin10Ver1803OrGreater() {
        return isWin10Ver(WIN10_1803_BUILD, VER_GREATER_EQUAL);
    }

    bool isWin10Ver1809OrGreater() {
        return isWin10Ver(WIN10_1809_BUILD, VER_GREATER_EQUAL);
    }

    bool isWin10Ver1903OrGreater() {
        return isWin10Ver(WIN10_1903_BUILD, VER_GREATER_EQUAL);
    }

}
}