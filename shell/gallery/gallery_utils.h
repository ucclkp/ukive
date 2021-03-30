// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_GALLERY_GALLERY_UTILS_H_
#define SHELL_GALLERY_GALLERY_UTILS_H_

#include <string_view>


namespace shell {

    bool isImageExtension(const std::u16string_view& ext);
    int compareLingString(const std::u16string& str1, const std::u16string& str2);

}

#endif  // SHELL_GALLERY_GALLERY_UTILS_H_