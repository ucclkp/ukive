// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shell/gallery/gallery_utils.h"

#include "utils/convert.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


namespace shell {

    bool isImageExtension(const std::u16string_view& ext) {
        auto lower_ext = utl::toASCIILower(ext);
        if (lower_ext == u".png" ||
            lower_ext == u".jpg" ||
            lower_ext == u".jpeg" ||
            lower_ext == u".bmp" ||
            lower_ext == u".gif")
        {
            return true;
        }
        return false;
    }

    int compareLingString(const std::u16string& str1, const std::u16string& str2) {
        return ::CompareStringW(
            LOCALE_USER_DEFAULT,
            SORT_DIGITSASNUMBERS,
            reinterpret_cast<PCNZWCH>(str1.c_str()), -1,
            reinterpret_cast<PCNZWCH>(str2.c_str()), -1);
    }

}
