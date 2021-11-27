// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_COLOR_MANAGEMENT_MODULE_H_
#define UKIVE_GRAPHICS_COLORS_COLOR_MANAGEMENT_MODULE_H_

#include "ukive/graphics/colors/color.h"


namespace ukive {

namespace icc {
    class CurveType;
    class ParamCurveType;
}

    class ColorManagementModule {
    public:
        enum class Intent {
            Perceptual,
            Saturation,
            AbsoluteColorimetric,
            RelativeColorimetric,
        };

        ColorManagementModule();
        ~ColorManagementModule();

        bool convertColor(
            const std::u16string& icc_path,
            Intent intent, const Color& src, Color* dst);

    private:
        struct CIEXYZ {
            float x, y, z;
        };

        struct Pos {
            float x, y;
        };

        struct RGBSystem {
            Pos red;
            Pos green;
            Pos blue;
            Pos ref_white;
        };

        static bool RGBToCIEXYZ(
            const Color& src, const RGBSystem& system, CIEXYZ* xyz);

        float calCurve(const icc::CurveType* curve, float val) const;
        float calInvCurve(const icc::CurveType* curve, float val) const;

        bool calParamCurve(const icc::ParamCurveType* curve, float val, float* out) const;
        bool calInvParamCurve(const icc::ParamCurveType* curve, float val, float* out) const;
    };

}

#endif  // UKIVE_GRAPHICS_COLORS_COLOR_MANAGEMENT_MODULE_H_