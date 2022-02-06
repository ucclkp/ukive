// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ucmm.h"

#include <cassert>
#include <filesystem>

#include "utils/numbers.hpp"

#include "ukive/graphics/colors/icc/icc_parser.h"
#include "ukive/graphics/colors/icc/icc_types.h"
#include "ukive/graphics/colors/icc/icc_constants.h"
#include "ukive/graphics/colors/icc/types/icc_type_curve.h"
#include "ukive/graphics/colors/icc/types/icc_type_param_curve.h"
#include "ukive/graphics/colors/icc/types/icc_type_xyz.h"


namespace {

    float fromU8F8(uint16_t u8f8) {
        return (u8f8 >> 8) + (u8f8 & 0xFFU) / 256.f;
    }

    float fromS15F16(uint32_t s15f16) {
        return int16_t(s15f16 >> 16) + (s15f16 & 0xFFFF) / 65536.f;
    }

}

namespace ukive {

    // static
    int UCMM::RGBToTarget(
        Intent intent,
        const Color& rgb, const RGBSystem& system,
        icc::ICCProfile& target, Color* dst)
    {
        /**
         * 基本步骤：
         * 1. 将 RGB 值转换到线性空间
         * 2. 转换到 CIE (ICC-absolute colorimetric) 值
         * 3. 检查白点是否匹配，若不匹配，进行色度匹配 (chromatic adaptation)
         * 4. 转换到 PCS (media-relative colorimetric) 值
         * 5. 转换到目标 RGB 值
         */

        Color l_rgb = rgb;
        RGBToLinear(system, &l_rgb);

        CIEXYZ xyz;
        if (!RGBToCIEXYZ(l_rgb, system, &xyz)) {
            return icc::ICCRC_NOT_INVERTIBLE;
        }

        icc::ICCParser::ProfileHeader header;
        int ret = target.getHeader(&header);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }

        CIEXYZ pcs_wp;
        pcs_wp.x = fromS15F16(header.pcs_illuminant_nCIEXYZ.x);
        pcs_wp.y = fromS15F16(header.pcs_illuminant_nCIEXYZ.y);
        pcs_wp.z = fromS15F16(header.pcs_illuminant_nCIEXYZ.z);

        CIEXYZ act_wp;
        act_wp.x = system.ref_white.x / system.ref_white.y;
        act_wp.y = 1.f;
        act_wp.z = (1 - system.ref_white.x - system.ref_white.y) / system.ref_white.y;

        icc::XYZNumber media_wp_num;
        ret = obtainXYZType(target, icc::kTagMediaWhitePoint, &media_wp_num);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }

        CIEXYZ media_wp;
        media_wp.x = fromS15F16(media_wp_num.x);
        media_wp.y = fromS15F16(media_wp_num.y);
        media_wp.z = fromS15F16(media_wp_num.z);

        if (!utl::is_num_equal(media_wp.x, act_wp.x) ||
            !utl::is_num_equal(media_wp.y, act_wp.y))
        {
            utl::math::MatrixT<float, 3, 3> m_adapt;
            if (!chromaticAdapt(media_wp, act_wp, &m_adapt)) {
                return icc::ICCRC_NOT_INVERTIBLE;
            }

            utl::math::MatrixT<float, 3, 1> v_src_xyz{
                xyz.x, xyz.y, xyz.z
            };

            auto v_dst_xyz = m_adapt * v_src_xyz;
            xyz.x = v_dst_xyz(0);
            xyz.y = v_dst_xyz(1);
            xyz.z = v_dst_xyz(2);
        }

        PCSXYZ pcs_xyz;
        CIEXYZToPCSXYZ(media_wp, pcs_wp, xyz, &pcs_xyz);

        /**
         * 根据 8.10：
         * 对于 Input, Display, Output 或 Color Space 配置文件，
         * 应用以下变换。
         */
        switch (header.profile_dev_class) {
        case icc::kClassDisplayDevice:
        case icc::kClassInputDevice:
        case icc::kClassOutputDevice:
        case icc::kClassColorSpace:
            ret = transformIDOCs(target, header, intent, pcs_xyz, dst);
            break;

        case icc::kClassDeviceLink:
        case icc::kClassAbstract:
            ret = transformDlA(target, header, intent, pcs_xyz, dst);
            break;

        case icc::kClassNamedColor:
        default:
            ret = icc::ICCRC_NO_DATA;
            break;
        }

        if (ret == icc::ICCRC_OK) {
            dst->a = rgb.a;
        }

        return ret;
    }

    // static
    int UCMM::sRGBToTarget(
        Intent intent,
        const Color& srgb,
        icc::ICCProfile& target, Color* dst)
    {
        /**
         * 基本步骤：
         * 1. 拿到待转换的 RGB 值
         * 2. 转换到 CIE (ICC-absolute colorimetric) 值
         * 3. 检查白点是否匹配，若不匹配，进行色度匹配 (chromatic adaptation)
         * 4. 转换到 PCS (media-relative colorimetric) 值
         * 5. 转换到目标 RGB 值
         */

        // ITU 709
        RGBSystem system;
        system.red.x = 0.6400f;
        system.red.y = 0.3300f;
        system.green.x = 0.3000f;
        system.green.y = 0.6000f;
        system.blue.x = 0.1500f;
        system.blue.y = 0.0600f;
        // D65
        system.ref_white.x = 0.3127f;
        system.ref_white.y = 0.3290f;

        Color l_rgb = srgb;
        sRGBToLinear(&l_rgb);

        CIEXYZ xyz;
        if (!RGBToCIEXYZ(l_rgb, system, &xyz)) {
            return icc::ICCRC_NOT_INVERTIBLE;
        }

        icc::ICCParser::ProfileHeader header;
        int ret = target.getHeader(&header);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }

        CIEXYZ pcs_wp;
        pcs_wp.x = fromS15F16(header.pcs_illuminant_nCIEXYZ.x);
        pcs_wp.y = fromS15F16(header.pcs_illuminant_nCIEXYZ.y);
        pcs_wp.z = fromS15F16(header.pcs_illuminant_nCIEXYZ.z);

        CIEXYZ act_wp;
        act_wp.x = system.ref_white.x / system.ref_white.y;
        act_wp.y = 1.f;
        act_wp.z = (1 - system.ref_white.x - system.ref_white.y) / system.ref_white.y;

        icc::XYZNumber media_wp_num;
        ret = obtainXYZType(target, icc::kTagMediaWhitePoint, &media_wp_num);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }

        CIEXYZ media_wp;
        media_wp.x = fromS15F16(media_wp_num.x);
        media_wp.y = fromS15F16(media_wp_num.y);
        media_wp.z = fromS15F16(media_wp_num.z);

        if (!utl::is_num_equal(media_wp.x, act_wp.x) ||
            !utl::is_num_equal(media_wp.z, act_wp.z))
        {
            utl::math::MatrixT<float, 3, 3> m_adapt;
            if (!chromaticAdapt(media_wp, act_wp, &m_adapt)) {
                return icc::ICCRC_NOT_INVERTIBLE;
            }

            utl::math::MatrixT<float, 3, 1> v_src_xyz{
                xyz.x, xyz.y, xyz.z
            };

            auto v_dst_xyz = m_adapt * v_src_xyz;
            xyz.x = v_dst_xyz(0);
            xyz.y = v_dst_xyz(1);
            xyz.z = v_dst_xyz(2);
        }

        PCSXYZ pcs_xyz;
        CIEXYZToPCSXYZ(media_wp, pcs_wp, xyz, &pcs_xyz);

        /**
         * 根据 8.10：
         * 对于 Input, Display, Output 或 Color Space 配置文件，
         * 应用以下变换。
         */
        switch (header.profile_dev_class) {
        case icc::kClassDisplayDevice:
        case icc::kClassInputDevice:
        case icc::kClassOutputDevice:
        case icc::kClassColorSpace:
            ret = transformIDOCs(target, header, intent, pcs_xyz, dst);
            break;

        case icc::kClassDeviceLink:
        case icc::kClassAbstract:
            ret = transformDlA(target, header, intent, pcs_xyz, dst);
            break;

        case icc::kClassNamedColor:
        default:
            ret = icc::ICCRC_NO_DATA;
            break;
        }

        if (ret == icc::ICCRC_OK) {
            dst->a = srgb.a;
        }

        return ret;
    }

    // static
    void UCMM::RGBToLinear(const RGBSystem& system, Color* rgb) {
        rgb->r = std::pow(rgb->r, system.red_gamma);
        rgb->g = std::pow(rgb->g, system.green_gamma);
        rgb->b = std::pow(rgb->b, system.blue_gamma);
    }

    // static
    void UCMM::sRGBToLinear(Color* srgb) {
        Color::sRGBToLinear(srgb);
    }

    // static
    bool UCMM::RGBToCIEXYZ(
        const Color& src, const RGBSystem& system, CIEXYZ* xyz)
    {
        auto Xr = system.red.x / system.red.y;
        auto Yr = 1.f;
        auto Zr = (1.f - system.red.x - system.red.y) / system.red.y;

        auto Xg = system.green.x / system.green.y;
        auto Yg = 1.f;
        auto Zg = (1.f - system.green.x - system.green.y) / system.green.y;

        auto Xb = system.blue.x / system.blue.y;
        auto Yb = 1.f;
        auto Zb = (1.f - system.blue.x - system.blue.y) / system.blue.y;

        utl::math::MatrixT<float, 3, 3> m{
            Xr, Xg, Xb,
            Yr, Yg, Yb,
            Zr, Zg, Zb,
        };

        utl::math::MatrixT<float, 3, 3> m_inv;
        if (!m.inverse(&m_inv)) {
            return false;
        }

        utl::math::MatrixT<float, 3, 1> W{
            system.ref_white.x / system.ref_white.y,
            1.f,
            (1.f - system.ref_white.x - system.ref_white.y) / system.ref_white.y
        };

        auto S = m_inv * W;

        utl::math::MatrixT<float, 3, 3> M{
            S(0) * Xr, S(1) * Xg, S(2) * Xb,
            S(0) * Yr, S(1) * Yg, S(2) * Yb,
            S(0) * Zr, S(1) * Zg, S(2) * Zb,
        };

        utl::math::MatrixT<float, 3, 1> SRC{
            src.r, src.g, src.b
        };

        auto DST = M * SRC;
        xyz->x = DST(0);
        xyz->y = DST(1);
        xyz->z = DST(2);

        return true;
    }

    // static
    bool UCMM::chromaticAdapt(
        const CIEXYZ& dst_wp, const CIEXYZ& src_wp, utl::math::MatrixT<float, 3, 3>* out)
    {
        // Linear Bradford CAT, 参见附录 E
        utl::math::MatrixT<float, 3, 3> m_BDF{
            0.8951f,  0.2664f,  -0.1614f,
            -0.7502f, 1.7135f,  0.0367f,
            0.0389f,  -0.0685f, 1.0296f,
        };

        utl::math::MatrixT<float, 3, 3> m_BDF_inv;
        if (!m_BDF.inverse(&m_BDF_inv)) {
            return false;
        }

        utl::math::MatrixT<float, 3, 1> v_naw{
            src_wp.x, src_wp.y, src_wp.z
        };

        utl::math::MatrixT<float, 3, 1> v_w{
            dst_wp.x, dst_wp.y, dst_wp.z
        };

        auto crv_src = m_BDF * v_naw;
        auto crv_pcs = m_BDF * v_w;

        utl::math::MatrixT<float, 3, 3> m_D{
            crv_pcs(0) / crv_src(0), 0, 0,
            0, crv_pcs(1) / crv_src(1), 0,
            0, 0, crv_pcs(2) / crv_src(2)
        };

        *out = m_BDF_inv * m_D * m_BDF;
        return true;
    }

    // static
    void UCMM::CIEXYZToPCSXYZ(
        const CIEXYZ& media_wp, const CIEXYZ& pcs_wp,
        const CIEXYZ& xyz, PCSXYZ* out)
    {
        out->x = xyz.x * (pcs_wp.x / media_wp.x);
        out->y = xyz.y * (pcs_wp.y / media_wp.y);
        out->z = xyz.z * (pcs_wp.z / media_wp.z);
    }

    // static
    int UCMM::transformIDOCs(
        icc::ICCProfile& profile,
        const icc::ICCParser::ProfileHeader& header, Intent intent,
        const PCSXYZ& src, Color* dev)
    {
        switch (intent) {
        case Intent::Perceptual:
        {
            const icc::ICCType* type;
            int ret = profile.obtainTagData(icc::kTagBToD0, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA0, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            icc::XYZNumber red_mat_num, green_mat_num, blue_mat_num;
            ret = obtainXYZType(profile, icc::kTagRedMatCol, &red_mat_num);
            if (ret != icc::ICCRC_OK) {
                return ret;
            }
            ret = obtainXYZType(profile, icc::kTagGreenMatCol, &green_mat_num);
            if (ret != icc::ICCRC_OK) {
                return ret;
            }
            ret = obtainXYZType(profile, icc::kTagBlueMatCol, &blue_mat_num);
            if (ret != icc::ICCRC_OK) {
                return ret;
            }

            utl::math::MatrixT<float, 3, 3> mat{
                fromS15F16(red_mat_num.x), fromS15F16(green_mat_num.x), fromS15F16(blue_mat_num.x),
                fromS15F16(red_mat_num.y), fromS15F16(green_mat_num.y), fromS15F16(blue_mat_num.y),
                fromS15F16(red_mat_num.z), fromS15F16(green_mat_num.z), fromS15F16(blue_mat_num.z),
            };

            utl::math::VectorT<float, 3> conn{
                src.x, src.y, src.z
            };

            {
                CIEXYZ pcs_wp;
                pcs_wp.x = fromS15F16(header.pcs_illuminant_nCIEXYZ.x);
                pcs_wp.y = fromS15F16(header.pcs_illuminant_nCIEXYZ.y);
                pcs_wp.z = fromS15F16(header.pcs_illuminant_nCIEXYZ.z);

                CIEXYZ act_wp;
                act_wp.x = 0.95047f;
                act_wp.y = 1.f;
                act_wp.z = 1.08883f;

                utl::math::MatrixT<float, 3, 3> m_adapt;
                chromaticAdapt(act_wp, pcs_wp, &m_adapt);
                conn = m_adapt * conn;
            }

            utl::math::MatrixT<float, 3, 3> mat_inv;
            if (!mat.inverse(&mat_inv)) {
                return icc::ICCRC_NOT_INVERTIBLE;
            }

            auto linear = mat_inv * conn;
            float red   = std::clamp(linear(0), 0.f, 1.f);
            float green = std::clamp(linear(1), 0.f, 1.f);
            float blue  = std::clamp(linear(2), 0.f, 1.f);

            ret = cpcInvTransform(profile, icc::kTagRedTRC, &red);
            if (ret != icc::ICCRC_OK) {
                return ret;
            }
            ret = cpcInvTransform(profile, icc::kTagGreenTRC, &green);
            if (ret != icc::ICCRC_OK) {
                return ret;
            }
            ret = cpcInvTransform(profile, icc::kTagBlueTRC, &blue);
            if (ret != icc::ICCRC_OK) {
                return ret;
            }

            dev->r = red;
            dev->g = green;
            dev->b = blue;
            return icc::ICCRC_OK;
        }

        case Intent::Saturation:
        {
            const icc::ICCType* type;
            int ret = profile.obtainTagData(icc::kTagBToD2, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA2, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA0, &type);
            break;
        }

        case Intent::AbsColor:
        {
            const icc::ICCType* type;
            int ret = profile.obtainTagData(icc::kTagBToD3, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA1, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA0, &type);
            break;
        }

        case Intent::RelColor:
        {
            const icc::ICCType* type;
            int ret = profile.obtainTagData(icc::kTagBToD1, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA1, &type);
            if (ret == icc::ICCRC_OK) {
                break;
            }

            ret = profile.obtainTagData(icc::kTagBToA0, &type);
            break;
        }

        default:
            return icc::ICCRC_NOT_IMPL;
        }

        return true;
    }

    // static
    int UCMM::transformDlA(
        icc::ICCProfile& profile,
        const icc::ICCParser::ProfileHeader& header, Intent intent,
        const PCSXYZ& src, Color* dev)
    {
        return icc::ICCRC_NOT_IMPL;
    }

    // static
    int UCMM::obtainXYZType(
        icc::ICCProfile& profile, uint32_t tag_sign, icc::XYZNumber* num)
    {
        const icc::ICCType* type;
        int ret = profile.obtainTagData(tag_sign, &type);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }
        if (type->getType() != icc::kTypeXYZ) {
            return icc::ICCRC_WRONG_TYPE;
        }

        auto& vals = static_cast<const icc::XYZType*>(type)->xyz_vals;
        if (vals.empty()) {
            return icc::ICCRC_NO_DATA;
        }

        *num = vals[0];
        return icc::ICCRC_OK;
    }

    // static
    int UCMM::cpcTransform(
        icc::ICCProfile& profile, uint32_t tag_sign, float* val)
    {
        const icc::ICCType* type;
        int ret = profile.obtainTagData(tag_sign, &type);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }

        if (type->getType() == icc::kTypeCurve) {
            *val = calCurve(static_cast<const icc::CurveType*>(type), *val);
        } else if (type->getType() == icc::kTypeParamCurve) {
            if (!calParamCurve(static_cast<const icc::ParamCurveType*>(type), *val, val)) {
                return icc::ICCRC_NO_DATA;
            }
        } else {
            return icc::ICCRC_WRONG_TYPE;
        }
        return icc::ICCRC_OK;
    }

    // static
    int UCMM::cpcInvTransform(
        icc::ICCProfile& profile, uint32_t tag_sign, float* val)
    {
        const icc::ICCType* type;
        int ret = profile.obtainTagData(tag_sign, &type);
        if (ret != icc::ICCRC_OK) {
            return ret;
        }

        if (type->getType() == icc::kTypeCurve) {
            *val = calInvCurve(static_cast<const icc::CurveType*>(type), *val);
        } else if (type->getType() == icc::kTypeParamCurve) {
            if (!calInvParamCurve(static_cast<const icc::ParamCurveType*>(type), *val, val)) {
                return icc::ICCRC_NO_DATA;
            }
        } else {
            return icc::ICCRC_WRONG_TYPE;
        }
        return icc::ICCRC_OK;
    }

    // static
    float UCMM::calCurve(const icc::CurveType* curve, float val) {
        if (curve->vals.empty()) {
            return val;
        }
        if (curve->vals.size() == 1) {
            float gamma = fromU8F8(curve->vals[0]);
            float result = std::pow(val, gamma);
            return result;
        }

        auto x = float((curve->vals.size() - 1) * val);
        uint32_t x_low = uint32_t(std::floor(x));
        uint32_t x_high = uint32_t(std::ceil(x));
        assert(x_high - x_low <= 1);

        uint16_t y_low = curve->vals[x_low];
        if (x_low == x_high) {
            return y_low / 65535.f;
        }

        uint16_t y_high = curve->vals[x_high];
        float y = y_low + (y_high - y_low) * (x - x_low);
        return y / 65535.f;
    }

    // static
    float UCMM::calInvCurve(const icc::CurveType* curve, float val) {
        if (curve->vals.empty()) {
            return val;
        }
        if (curve->vals.size() == 1) {
            float gamma = fromU8F8(curve->vals[0]);
            float result = std::pow(val, 1 / gamma);
            return result;
        }

        auto y = float(65535.f * val);
        uint16_t y_low = uint16_t(std::floor(y));
        uint16_t y_high = uint16_t(std::ceil(y));
        assert(y_high - y_low <= 1);

        size_t x_low = 0;
        size_t x_high = 0;
        int32_t low_diff = 0x7FFFFFFF;
        int32_t high_diff = 0x7FFFFFFF;
        size_t num = curve->vals.size();
        for (size_t x_i = 0; x_i < num; ++x_i) {
            uint16_t y_i = curve->vals[x_i];
            int32_t ld = std::abs(y_i - y_low);
            if (ld <= low_diff) {
                x_low = x_i;
                low_diff = ld;
            }
            int32_t hd = std::abs(y_i - y_high);
            if (hd <= high_diff) {
                x_high = x_i;
                high_diff = hd;
            }
        }

        if (x_low == x_high) {
            return float(x_low) / (num - 1);
        }

        float x = x_low + (x_high - x_low) * (y - y_low);
        return x / (num - 1);
    }

    // static
    bool UCMM::calParamCurve(
        const icc::ParamCurveType* curve, float val, float* out)
    {
        switch (curve->func_type) {
        case 0x0000:
        {
            float g = fromS15F16(curve->g);
            *out = std::pow(val, g);
            break;
        }

        case 0x0001:
        {
            float a = fromS15F16(curve->a);
            float b = fromS15F16(curve->b);
            float det = -b / a;
            if (val >= det) {
                float g = fromS15F16(curve->g);
                *out = std::pow(a * val + b, g);
            } else {
                *out = 0;
            }
            break;
        }

        case 0x0002:
        {
            float a = fromS15F16(curve->a);
            float b = fromS15F16(curve->b);
            float c = fromS15F16(curve->c);
            float det = -b / a;
            if (val >= det) {
                float g = fromS15F16(curve->g);
                *out = std::pow(a * val + b, g) + c;
            } else {
                *out = c;
            }
            break;
        }

        case 0x0003:
        {
            float d = fromS15F16(curve->d);
            if (val >= d) {
                float a = fromS15F16(curve->a);
                float b = fromS15F16(curve->b);
                float g = fromS15F16(curve->g);
                *out = std::pow(a * val + b, g);
            } else {
                float c = fromS15F16(curve->c);
                *out = c * val;
            }
            break;
        }

        case 0x0004:
        {
            float c = fromS15F16(curve->c);
            float d = fromS15F16(curve->d);
            if (val >= d) {
                float a = fromS15F16(curve->a);
                float b = fromS15F16(curve->b);
                float g = fromS15F16(curve->g);
                *out = std::pow(a * val + b, g) + c;
            } else {
                float f = fromS15F16(curve->f);
                *out = c * val + f;
            }
            break;
        }

        default:
            assert(false);
            return false;
        }

        if (*out < 0) { *out = 0; }
        if (*out > 1) { *out = 1; }
        return true;
    }

    // static
    bool UCMM::calInvParamCurve(
        const icc::ParamCurveType* curve, float val, float* out)
    {
        switch (curve->func_type) {
        case 0x0000:
        {
            float g = fromS15F16(curve->g);
            *out = std::pow(val, 1 / g);
            break;
        }

        case 0x0001:
        {
            float a = fromS15F16(curve->a);
            float b = fromS15F16(curve->b);
            float g = fromS15F16(curve->g);
            *out = (std::pow(val, 1 / g) - b) / a;
            break;
        }

        case 0x0002:
        {
            float a = fromS15F16(curve->a);
            float b = fromS15F16(curve->b);
            float c = fromS15F16(curve->c);
            float g = fromS15F16(curve->g);
            if (val < c) {
                val = c;
            }
            *out = (std::pow(val - c, 1 / g) - b) / a;
            break;
        }

        case 0x0003:
        {
            float a = fromS15F16(curve->a);
            float b = fromS15F16(curve->b);
            float c = fromS15F16(curve->c);
            float d = fromS15F16(curve->d);
            float g = fromS15F16(curve->g);
            if (val < c * d) {
                val = c * d;
            }
            *out = (std::pow(val, 1 / g) - b) / a;
            break;
        }

        case 0x0004:
        {
            float a = fromS15F16(curve->a);
            float b = fromS15F16(curve->b);
            float c = fromS15F16(curve->c);
            float d = fromS15F16(curve->d);
            float f = fromS15F16(curve->f);
            float g = fromS15F16(curve->g);
            if (val < c * d + f) {
                val = c * d + f;
            }
            *out = (std::pow(val - c, 1 / g) - b) / a;
            break;
        }

        default:
            assert(false);
            return false;
        }

        if (*out < 0) { *out = 0; }
        if (*out > 1) { *out = 1; }
        return true;
    }

    // static
    void UCMM::nCIEXYZ(CIEXYZ* xyz) {
        xyz->x /= xyz->y;
        xyz->z /= xyz->y;
        xyz->y = 1.f;
    }

}
