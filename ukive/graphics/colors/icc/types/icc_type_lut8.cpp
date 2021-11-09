// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_lut8.h"


namespace ukive {
namespace icc {

    Lut8Type::Lut8Type(uint32_t type)
        : ICCType(type) {}

    bool Lut8Type::onParse(std::istream& s, uint32_t size) {
        return false;
    }


}
}
