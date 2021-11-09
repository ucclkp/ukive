// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_xyz.h"

#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    XYZType::XYZType(uint32_t type)
        : ICCType(type) {}

    bool XYZType::onParse(std::istream& s, uint32_t size) {
        if (size < 4) {
            return false;
        }

        SKIP_BYTES(4);

        size_t count = (size - 4) / (4 * 3);
        xyz_vals.resize(count);

        for (size_t i = 0; i < count; ++i) {
            READ_STREAM_BE(xyz_vals[i].x, 4);
            READ_STREAM_BE(xyz_vals[i].y, 4);
            READ_STREAM_BE(xyz_vals[i].z, 4);
        }

        return true;
    }

}
}
