// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_lut_a2b.h"

#include <cassert>

#include "utils/stream_utils.h"

#include "ukive/graphics/colors/icc/icc_constants.h"
#include "ukive/graphics/colors/icc/types/icc_type_curve.h"
#include "ukive/graphics/colors/icc/types/icc_type_param_curve.h"


namespace ukive {
namespace icc {

    LutAToBType::LutAToBType(uint32_t type)
        : ICCType(type)
    {}

    LutAToBType::~LutAToBType() {}

    bool LutAToBType::onParse(std::istream& s, uint32_t size) {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);

        READ_STREAM(in_channel_num, 1);
        READ_STREAM(out_channel_num, 1);
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

        // B curve
        if (B_curve_offset != 0) {
            bool finished = false;
            uint32_t cur_off = B_curve_offset + (4 - (B_curve_offset % 4));
            SEEKG_STREAM(start_pos + std::streamoff(cur_off - 4));
            for (uint8_t i = 0; i < out_channel_num; ++i) {
                uint32_t type_sign;
                READ_STREAM_BE(type_sign, 4);
                switch (type_sign) {
                case kTypeCurve:
                    break;

                case kTypeParamCurve:
                    break;

                default:
                    assert(false);
                    finished = true;
                    break;
                }

                if (finished) {
                    break;
                }

                auto pos = uint32_t(s.tellg());
                SKIP_BYTES(4 - (pos % 4));
            }
        }
    }

}
}
