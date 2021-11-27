// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_lut_ab.h"

#include <cassert>

#include "utils/aligment.hpp"
#include "utils/stl_utils.h"
#include "utils/stream_utils.h"

#include "ukive/graphics/colors/icc/icc_constants.h"
#include "ukive/graphics/colors/icc/types/icc_type_curve.h"
#include "ukive/graphics/colors/icc/types/icc_type_param_curve.h"


namespace ukive {
namespace icc {

    LutABType::LutABType(uint32_t type)
        : ICCType(type)
    {}

    LutABType::~LutABType() {
        delete[] clut8;
        delete[] clut16;
        utl::STLDeleteElements(&A_curve);
        utl::STLDeleteElements(&B_curve);
        utl::STLDeleteElements(&M_curve);
    }

    bool LutABType::onParse(std::istream& s, uint32_t size) {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);

        READ_STREAM(in_num, 1);
        READ_STREAM(out_num, 1);
        SKIP_BYTES(2);

        uint32_t B_curve_offset;
        READ_STREAM_BE(B_curve_offset, 4);

        uint32_t matrix_offset;
        READ_STREAM_BE(matrix_offset, 4);

        uint32_t M_curve_offset;
        READ_STREAM_BE(M_curve_offset, 4);

        uint32_t clut_offset;
        READ_STREAM_BE(clut_offset, 4);

        uint32_t A_curve_offset;
        READ_STREAM_BE(A_curve_offset, 4);

        // A curve
        if (A_curve_offset != 0) {
            uint32_t cur_off = utl::align4(A_curve_offset);
            SEEKG_STREAM(start_pos + std::streamoff(cur_off - 4));
            if (!parseCurves(s, in_num, &A_curve)) {
                return false;
            }
        }

        // B curve
        if (B_curve_offset != 0) {
            if (B_curve_offset == A_curve_offset) {
                B_curve = A_curve;
            } else {
                uint32_t cur_off = utl::align4(B_curve_offset);
                SEEKG_STREAM(start_pos + std::streamoff(cur_off - 4));
                if (!parseCurves(s, out_num, &B_curve)) {
                    return false;
                }
            }
        }

        // M curve
        if (M_curve_offset != 0) {
            if (M_curve_offset == A_curve_offset) {
                M_curve = A_curve;
            } else if (M_curve_offset == B_curve_offset) {
                M_curve = B_curve;
            } else {
                uint32_t cur_off = utl::align4(M_curve_offset);
                SEEKG_STREAM(start_pos + std::streamoff(cur_off - 4));
                if (!parseCurves(s, out_num, &M_curve)) {
                    return false;
                }
            }
        }

        // CLUT
        if (clut_offset != 0) {
            SEEKG_STREAM(start_pos + std::streamoff(clut_offset - 4));
            uint8_t gp_nums[16];
            if (in_num > 16) {
                return false;
            }
            READ_STREAM(gp_nums, in_num);
            SKIP_BYTES(16Ui8 - in_num);

            uint8_t precision;
            READ_STREAM(precision, 1);
            SKIP_BYTES(3);

            size_t element_size = out_num;
            for (uint8_t i = 0; i < in_num; ++i) {
                element_size *= gp_nums[i];
            }
            clut_element_count = element_size;

            if (precision == 0x01) {
                clut8 = new uint8_t[element_size];
                READ_STREAM(clut8, element_size);
            } else if (precision == 0x02) {
                clut16 = new uint16_t[element_size];
                for (uint8_t i = 0; i < element_size; ++i) {
                    READ_STREAM_BE(clut16[i], 2);
                }
            } else {
                assert(false);
                return false;
            }
        } else {
            if (in_num != out_num) {
                return false;
            }
        }

        // Matrix
        if (matrix_offset != 0) {
            has_matrix = true;
            SEEKG_STREAM(start_pos + std::streamoff(clut_offset - 4));
            for (size_t i = 0; i < 12; ++i) {
                READ_STREAM_BE(matrix[i], 4);
            }
        }

        return true;
    }

    bool LutABType::onCheckSize(uint32_t read_size, uint32_t total_size) const {
        return read_size <= total_size;
    }

    bool LutABType::parseCurves(
        std::istream& s, uint8_t channel_num, std::vector<ICCType*>* out)
    {
        bool finished = false;
        for (uint8_t i = 0; i < channel_num; ++i) {
            uint32_t type_sign;
            READ_STREAM_BE(type_sign, 4);
            switch (type_sign) {
            case kTypeCurve:
            {
                auto curve = new CurveType(type_sign);
                if (curve->parse(s)) {
                    out->push_back(curve);
                } else {
                    delete curve;
                    return false;
                }
                break;
            }

            case kTypeParamCurve:
            {
                auto curve = new ParamCurveType(type_sign);
                if (curve->parse(s)) {
                    out->push_back(curve);
                } else {
                    delete curve;
                    return false;
                }
                break;
            }

            default:
                assert(false);
                finished = true;
                break;
            }

            if (finished) {
                break;
            }

            SKIP_BYTES(utl::align4off(uint32_t(s.tellg())));
        }

        return true;
    }

}
}
