// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_PARAM_CURVE_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_PARAM_CURVE_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <istream>


namespace ukive {
namespace icc {

    class ParamCurveType : public ICCType {
    public:
        explicit ParamCurveType(uint32_t type);
        ~ParamCurveType();

        uint16_t func_type;
        uint32_t g, a, b, c, d, e, f;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_PARAM_CURVE_H_