// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/gallery_utils.h"

#include "utils/strings/string_utils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


namespace shell {

    bool isImageExtension(const std::u16string_view& ext) {
        if (utl::isLitEqual(ext, u".png") ||
            utl::isLitEqual(ext, u".jpg") ||
            utl::isLitEqual(ext, u".jpeg") ||
            utl::isLitEqual(ext, u".bmp") ||
            utl::isLitEqual(ext, u".gif"))
        {
            return true;
        }
        return false;
    }

    int compareLingString(const std::u16string& str1, const std::u16string& str2) {
        std::wstring wstr1(str1.begin(), str1.end());
        std::wstring wstr2(str2.begin(), str2.end());

        return ::CompareStringW(
            LOCALE_USER_DEFAULT, SORT_DIGITSASNUMBERS,
            wstr1.c_str(), -1, wstr2.c_str(), -1);
    }

}
