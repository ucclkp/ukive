// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type.h"

#include <cassert>


namespace ukive {
namespace icc {

    ICCType::ICCType(uint32_t type)
        : type_(type) {}

    bool ICCType::parse(std::istream& s, uint32_t size) {
        assert(size >= 4);
        is_parsed_ = false;
        auto start_pos = s.tellg();

        if (onParse(s, size - 4)) {
            if (s.tellg() - start_pos + 4 != size) {
                return false;
            }
            is_parsed_ = true;
        }

        return is_parsed_;
    }

    bool ICCType::isParsed() const {
        return is_parsed_;
    }

    uint32_t ICCType::getType() const {
        return type_;
    }

}
}
