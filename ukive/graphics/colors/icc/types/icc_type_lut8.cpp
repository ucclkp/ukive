// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_lut8.h"

#include "utils/numbers.hpp"
#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    Lut8Type::Lut8Type(uint32_t type)
        : ICCType(type) {}

    bool Lut8Type::onParse(std::istream& s, uint32_t size) {
        SKIP_BYTES(4);
        READ_STREAM(in_channel_num, 1);
        READ_STREAM(out_channel_num, 1);
        READ_STREAM(clut_grid_pt_num, 1);
        SKIP_BYTES(1);
        READ_STREAM_BE(mat[0], 4);
        READ_STREAM_BE(mat[1], 4);
        READ_STREAM_BE(mat[2], 4);
        READ_STREAM_BE(mat[3], 4);
        READ_STREAM_BE(mat[4], 4);
        READ_STREAM_BE(mat[5], 4);
        READ_STREAM_BE(mat[6], 4);
        READ_STREAM_BE(mat[7], 4);
        READ_STREAM_BE(mat[8], 4);
        READ_STREAM_BE(in_tab_entry_num, 2);
        READ_STREAM_BE(out_tab_entry_num, 2);

        uint32_t it_num = uint32_t(in_tab_entry_num) * in_channel_num;
        in_tabs.resize(it_num);
        READ_STREAM(in_tabs[0], it_num);

        uint32_t cv_num = utl::powui<uint32_t>(clut_grid_pt_num, in_channel_num) * out_channel_num;
        clut_vals.resize(cv_num);
        READ_STREAM(clut_vals[0], cv_num);

        uint32_t out_num = uint32_t(out_tab_entry_num) * out_channel_num;
        out_tabs.resize(out_num);
        READ_STREAM(out_tabs[0], out_num);

        return true;
    }


}
}
