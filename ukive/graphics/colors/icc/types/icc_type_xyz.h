// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_XYZ_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_XYZ_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <vector>


namespace ukive {
namespace icc {

    class XYZType : public ICCType {
    public:
        explicit XYZType(uint32_t type);

        std::vector<XYZNumber> xyz_vals;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_XYZ_H_