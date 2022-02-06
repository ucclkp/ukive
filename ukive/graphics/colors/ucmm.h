// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_UCMM_H_
#define UKIVE_GRAPHICS_COLORS_UCMM_H_

#include "utils/math/algebra/matrix.hpp"

#include "ukive/graphics/colors/color.h"
#include "ukive/graphics/colors/icc/icc_profile.h"


namespace ukive {

namespace icc {
    class CurveType;
    class ParamCurveType;
}

    class UCMM {
    public:
        enum class Intent {
            Perceptual,
            Saturation,
            AbsColor,
            RelColor,
        };

        struct Pos {
            float x, y;
        };

        struct RGBSystem {
            Pos red;
            Pos green;
            Pos blue;
            Pos ref_white;
            float red_gamma;
            float green_gamma;
            float blue_gamma;
        };

        static int RGBToTarget(
            Intent intent,
            const Color& rgb, const RGBSystem& system,
            icc::ICCProfile& target, Color* dst);
        static int sRGBToTarget(
            Intent intent,
            const Color& srgb,
            icc::ICCProfile& target, Color* dst);

    private:
        struct CIEXYZ {
            float x, y, z;
        };

        struct PCSXYZ {
            float x, y, z;
        };

        static void RGBToLinear(const RGBSystem& system, Color* rgb);
        static void sRGBToLinear(Color* srgb);
        static bool RGBToCIEXYZ(
            const Color& src, const RGBSystem& system, CIEXYZ* xyz);
        static bool chromaticAdapt(
            const CIEXYZ& dst_wp, const CIEXYZ& src_wp, utl::math::MatrixT<float, 3, 3>* out);
        static void CIEXYZToPCSXYZ(
            const CIEXYZ& media_wp, const CIEXYZ& pcs_wp,
            const CIEXYZ& xyz, PCSXYZ* out);

        static int transformIDOCs(
            icc::ICCProfile& profile,
            const icc::ICCParser::ProfileHeader& header, Intent intent,
            const PCSXYZ& src, Color* dev);
        static int transformDlA(
            icc::ICCProfile& profile,
            const icc::ICCParser::ProfileHeader& header, Intent intent,
            const PCSXYZ& src, Color* dev);

        static int obtainXYZType(
            icc::ICCProfile& profile, uint32_t tag_sign, icc::XYZNumber* num);
        static int cpcTransform(
            icc::ICCProfile& profile, uint32_t tag_sign, float* val);
        static int cpcInvTransform(
            icc::ICCProfile& profile, uint32_t tag_sign, float* val);

        static float calCurve(const icc::CurveType* curve, float val);
        static float calInvCurve(const icc::CurveType* curve, float val);

        static bool calParamCurve(
            const icc::ParamCurveType* curve, float val, float* out);
        static bool calInvParamCurve(
            const icc::ParamCurveType* curve, float val, float* out);

        static void nCIEXYZ(CIEXYZ* xyz);
    };

}

#endif  // UKIVE_GRAPHICS_COLORS_UCMM_H_