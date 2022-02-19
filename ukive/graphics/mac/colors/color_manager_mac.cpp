// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color_manager_mac.h"

#include "utils/numbers.hpp"


namespace ukive {
namespace mac {

    ColorManagerMac::ColorManagerMac() {}

    bool ColorManagerMac::convertColor(const Color& src, Color* dst) {
        return false;
    }

    // static
    bool ColorManagerMac::getDefaultProfile(std::u16string* path) {
        return false;
    }

}
}
