// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT_AB_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT_AB_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <istream>
#include <vector>


namespace ukive {
namespace icc {

    class LutABType : public ICCType {
    public:
        explicit LutABType(uint32_t type);
        ~LutABType();

        uint8_t in_num = 0;
        uint8_t out_num = 0;
        std::vector<ICCType*> A_curve;
        std::vector<ICCType*> B_curve;
        std::vector<ICCType*> M_curve;
        uint8_t* clut8 = nullptr;
        uint16_t* clut16 = nullptr;
        size_t clut_element_count = 0;

        bool has_matrix = false;
        uint32_t matrix[12];

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
        bool onCheckSize(uint32_t read_size, uint32_t total_size) const override;

    private:
        bool parseCurves(
            std::istream& s, uint8_t channel_num, std::vector<ICCType*>* out);
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_LUT_AB_H_