// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_s15f16_array.h"

#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    S15F16ArrayType::S15F16ArrayType(uint32_t type)
        : ICCType(type)
    {}

    S15F16ArrayType::~S15F16ArrayType() {}

    bool S15F16ArrayType::onParse(std::istream& s, uint32_t size) {
        if (size < 4) {
            return false;
        }

        SKIP_BYTES(4);

        size_t count = (size - 4) / (4 * 3);
        vals.resize(count);

        for (size_t i = 0; i < count; ++i) {
            READ_STREAM_BE(vals[i], 4);
        }

        return true;
    }

}
}
