// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color_management_module.h"

#include <cassert>
#include <filesystem>
#include <fstream>

#include "ukive/graphics/colors/icc/icc_parser.h"
#include "ukive/graphics/colors/icc/icc_constants.h"
#include "ukive/graphics/colors/icc/types/icc_type_curve.h"
#include "ukive/graphics/colors/icc/types/icc_type_param_curve.h"


namespace {

    float fromU8F8(uint16_t u8f8) {
        return (u8f8 >> 8) + (u8f8 & 0xFFU) / 256.f;
    }

    float fromS15F16(uint32_t s15f16) {
        return int16_t(s15f16 >> 16) + (s15f16 & 0xFFFF) / 65535.f;
    }

}

namespace ukive {

    ColorManagementModule::ColorManagementModule() {}

    ColorManagementModule::~ColorManagementModule() {}

    bool ColorManagementModule::convertColor(
        const std::u16string& icc_path,
        Intent intent, const Color& src, Color* dst)
    {
        // RGB -> CIE -> PCS -> Target
        std::ifstream icc_file(std::filesystem::path(icc_path), std::ios::binary);
        if (!icc_file) {
            return false;
        }

        icc::ICCParser parser;
        if (!parser.parse(icc_file)) {
            return false;
        }

        auto& header = parser.getHeader();

        switch (header.profile_dev_class) {
        case icc::kClassDisplayDevice:
        case icc::kClassInputDevice:
        case icc::kClassOutputDevice:
        case icc::kClassColorSpace:
        {
            switch (intent) {
            case Intent::Perceptual:
            {
                auto type = parser.parseTagData(icc_file, icc::kTagBToD0);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA0);
                if (type) {
                    break;
                }

                auto r_trc = parser.parseTagData(icc_file, icc::kTagRedTRC);
                auto g_trc = parser.parseTagData(icc_file, icc::kTagGreenTRC);
                auto b_trc = parser.parseTagData(icc_file, icc::kTagBlueTRC);

                auto r_mc = parser.parseTagData(icc_file, icc::kTagRedMatCol);
                auto g_mc = parser.parseTagData(icc_file, icc::kTagGreenMatCol);
                auto b_mc = parser.parseTagData(icc_file, icc::kTagBlueMatCol);
                break;
            }

            case Intent::Saturation:
            {
                auto type = parser.parseTagData(icc_file, icc::kTagBToD2);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA2);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA0);
                break;
            }

            case Intent::AbsoluteColorimetric:
            {
                auto type = parser.parseTagData(icc_file, icc::kTagBToD3);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA1);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA0);
                break;
            }

            case Intent::RelativeColorimetric:
            {
                auto type = parser.parseTagData(icc_file, icc::kTagBToD1);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA1);
                if (type) {
                    break;
                }

                type = parser.parseTagData(icc_file, icc::kTagBToA0);
                break;
            }

            default:
                return false;
            }
            break;
        }

        default:
            return false;
        }

        auto type = parser.parseTagData(icc_file, icc::kTagRedTRC);
        if (type->getType() == icc::kTypeCurve) {
            float val = calCurve(static_cast<const icc::CurveType*>(type), 0.5);
            dst->r = val;
        } else if (type->getType() == icc::kTypeParamCurve) {
            float val;
            if (!calParamCurve(static_cast<const icc::ParamCurveType*>(type), 0.5, &val)) {
                return false;
            }
            dst->r = val;
        }

        return true;
    }

    // static
    bool ColorManagementModule::RGBToCIEXYZ(
        const Color& src, const RGBSystem& system, CIEXYZ* xyz)
    {
        return false;
    }

    float ColorManagementModule::calCurve(const icc::CurveType* curve, float val) const {
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

    float ColorManagementModule::calInvCurve(const icc::CurveType* curve, float val) const {
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

    bool ColorManagementModule::calParamCurve(
        const icc::ParamCurveType* curve, float val, float* out) const
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

    bool ColorManagementModule::calInvParamCurve(
        const icc::ParamCurveType* curve, float val, float* out) const
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

}
