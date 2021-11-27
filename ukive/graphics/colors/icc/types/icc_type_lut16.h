// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT16_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT16_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <vector>


namespace ukive {
namespace icc {

    class Lut16Type : public ICCType {
    public:
        explicit Lut16Type(uint32_t type);

        uint8_t in_channel_num = 0;
        uint8_t out_channel_num = 0;
        uint8_t clut_grid_pt_num = 0;
        uint32_t mat[9];
        uint16_t in_tab_entry_num = 0;
        uint16_t out_tab_entry_num = 0;
        std::vector<uint16_t> in_tabs;
        std::vector<uint16_t> clut_vals;
        std::vector<uint16_t> out_tabs;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT16_H_