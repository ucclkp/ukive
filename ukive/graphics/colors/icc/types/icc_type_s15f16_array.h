// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_S15F16_ARRAY_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_S15F16_ARRAY_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <istream>
#include <vector>


namespace ukive {
namespace icc {

    class S15F16ArrayType : public ICCType {
    public:
        explicit S15F16ArrayType(uint32_t type);
        ~S15F16ArrayType();

        std::vector<uint32_t> vals;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_S15F16_ARRAY_H_