// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT_A2B_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT_A2B_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <istream>


namespace ukive {
namespace icc {

    class LutAToBType : public ICCType {
    public:
        explicit LutAToBType(uint32_t type);
        ~LutAToBType();

        uint8_t in_channel_num;
        uint8_t out_channel_num;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT_A2B_H_