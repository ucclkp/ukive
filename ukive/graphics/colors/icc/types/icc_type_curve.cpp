// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_curve.h"

#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    CurveType::CurveType(uint32_t type)
        : ICCType(type)
    {}

    CurveType::~CurveType() {}

    bool CurveType::onParse(std::istream& s, uint32_t size) {
        SKIP_BYTES(4);

        uint32_t num;
        READ_STREAM_BE(num, 4);
        vals.resize(num);

        for (uint32_t i = 0; i < num; ++i) {
            READ_STREAM_BE(vals[i], 2);
        }

        return true;
    }

}
}
