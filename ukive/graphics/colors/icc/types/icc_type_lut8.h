// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT8_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT8_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"


namespace ukive {
namespace icc {

    class Lut8Type : public ICCType {
    public:
        explicit Lut8Type(uint32_t type);

    protected:
        bool onParse(std::istream& s, uint32_t size) override;

    private:

    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT8_H_