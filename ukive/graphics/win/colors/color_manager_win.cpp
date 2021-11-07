// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color_manager_win.h"

#include "ukive/graphics/win/display_win.h"


namespace ukive {

    ColorManagerWin::ColorManagerWin() {}

    bool ColorManagerWin::convertColor(const Color& src, Color* dst) {
        LOGCOLORSPACEW lcs;
        lcs.lcsSignature = LCS_SIGNATURE;
        lcs.lcsVersion = 0x400;
        lcs.lcsSize = sizeof(LOGCOLORSPACEW);
        lcs.lcsCSType = LCS_sRGB;
        lcs.lcsIntent = LCS_GM_GRAPHICS;
        std::memset(&lcs.lcsEndpoints, 0, sizeof(lcs.lcsEndpoints));
        lcs.lcsGammaRed = 0;
        lcs.lcsGammaGreen = 0;
        lcs.lcsGammaBlue = 0;
        lcs.lcsFilename[0] = '\0';

        auto display_profile = getDisplayProfile();
        HTRANSFORM transform = ::CreateColorTransformW(&lcs, display_profile, nullptr, 0);
        if (!transform) {
            ::CloseColorProfile(display_profile);
            return false;
        }

        COLOR input2;
        input2.rgb.red = WORD(std::round(src.r * 255)) << 8;
        input2.rgb.green = WORD(std::round(src.g * 255)) << 8;
        input2.rgb.blue = WORD(std::round(src.b * 255)) << 8;
        COLOR output2;
        BOOL ret = ::TranslateColors(transform, &input2, 1, COLOR_RGB, &output2, COLOR_RGB);
        if (ret != TRUE) {
            auto err = ::GetLastError();
            ::DeleteColorTransform(transform);
            ::CloseColorProfile(display_profile);
            return false;
        }

        dst->r = (output2.rgb.red >> 8) / 255.f;
        dst->g = (output2.rgb.green >> 8) / 255.f;
        dst->b = (output2.rgb.blue >> 8) / 255.f;
        dst->a = src.a;

        ::DeleteColorTransform(transform);
        ::CloseColorProfile(display_profile);
        return true;
    }

    HPROFILE ColorManagerWin::getDisplayProfile() {
        std::wstring display_icm;

        auto display = Display::fromPrimary();
        if (!static_cast<DisplayWin*>(display.get())->getICMProfilePath(&display_icm)) {
            return nullptr;
        }

        PROFILE profile{
            PROFILE_FILENAME,
            PVOID(display_icm.c_str()),
            (display_icm.size() + 1) * sizeof(WCHAR) };
        HPROFILE cf = ::WcsOpenColorProfileW(
            &profile, nullptr, nullptr, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
        return cf;
    }

    // static
    bool ColorManagerWin::getICMProfile(HDC hdc, std::wstring* path) {
        bool ret = false;
        WCHAR buf[MAX_PATH];
        DWORD length = MAX_PATH;
        if (::GetICMProfileW(hdc, &length, buf) != TRUE) {
            if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                std::wstring dbuf;
                dbuf.resize(length);
                if (::GetICMProfileW(hdc, &length, &*dbuf.begin()) == TRUE) {
                    *path = std::move(dbuf);
                    ret = true;
                }
            }
        } else {
            path->assign(buf, length);
            ret = true;
        }
        return ret;
    }

    // static
    bool ColorManagerWin::getDefaultProfile(std::wstring* path) {
        DWORD length = 0;
        BOOL ret = ::GetStandardColorSpaceProfileW(
            nullptr,
            LCS_sRGB,
            nullptr,
            &length);
        DWORD err = ::GetLastError();
        if (length == 0 || err != ERROR_INSUFFICIENT_BUFFER) {
            return false;
        }

        std::wstring dbuf;
        dbuf.resize((length + 1) / 2);

        ret = ::GetStandardColorSpaceProfileW(
            nullptr,
            LCS_sRGB,
            &*dbuf.begin(),
            &length);
        if (ret != TRUE) {
            return false;
        }

        if (!dbuf.empty() && dbuf.back() == L'\0') {
            dbuf.pop_back();
        }

        *path = std::move(dbuf);
        return true;
    }

}
