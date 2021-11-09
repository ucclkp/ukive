// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_param_curve.h"

#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    ParamCurveType::ParamCurveType(uint32_t type)
        : ICCType(type)
    {}

    ParamCurveType::~ParamCurveType() {}

    bool ParamCurveType::onParse(std::istream& s, uint32_t size) {
        SKIP_BYTES(4);

        READ_STREAM_BE(func_type, 2);
        SKIP_BYTES(2);

        switch (func_type) {
        case 0x0000:
            READ_STREAM_BE(g, 4);
            break;

        case 0x0001:
            READ_STREAM_BE(g, 4);
            READ_STREAM_BE(a, 4);
            READ_STREAM_BE(b, 4);
            break;

        case 0x0002:
            READ_STREAM_BE(g, 4);
            READ_STREAM_BE(a, 4);
            READ_STREAM_BE(b, 4);
            READ_STREAM_BE(c, 4);
            break;

        case 0x0003:
            READ_STREAM_BE(g, 4);
            READ_STREAM_BE(a, 4);
            READ_STREAM_BE(b, 4);
            READ_STREAM_BE(c, 4);
            READ_STREAM_BE(d, 4);
            break;

        case 0x0004:
            READ_STREAM_BE(g, 4);
            READ_STREAM_BE(a, 4);
            READ_STREAM_BE(b, 4);
            READ_STREAM_BE(c, 4);
            READ_STREAM_BE(d, 4);
            READ_STREAM_BE(e, 4);
            READ_STREAM_BE(f, 4);
            break;

        default:
            return false;
        }

        return true;
    }

}
}
