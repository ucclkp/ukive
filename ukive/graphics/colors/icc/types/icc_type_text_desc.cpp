// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_text_desc.h"

#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    TextDescType::TextDescType(uint32_t type)
        : ICCType(type) {}

    bool TextDescType::onParse(std::istream& s, uint32_t size) {
        SKIP_BYTES(4);

        uint32_t ascii_count;
        READ_STREAM_BE(ascii_count, 4);
        ascii_text.resize(ascii_count);
        if (ascii_count > 0) {
            READ_STREAM(ascii_text[0], ascii_count);
            ascii_text.pop_back();
        }

        READ_STREAM_BE(unicode_lang_code, 4);

        uint32_t uld_count;
        READ_STREAM_BE(uld_count, 4);
        unicode_loc_desc.resize(uld_count);
        if (uld_count > 0) {
            for (uint32_t i = 0; i < uld_count; ++i) {
                READ_STREAM_BE(unicode_loc_desc[i], 2);
            }
            unicode_loc_desc.pop_back();
        }

        READ_STREAM_BE(script_code, 2);

        uint8_t lmd_count;
        READ_STREAM(lmd_count, 1);
        loc_mac_desc.resize(lmd_count);
        if (lmd_count > 67) {
            return false;
        }

        if (lmd_count > 0) {
            READ_STREAM(loc_mac_desc[0], lmd_count);
            loc_mac_desc.pop_back();
        }

        SKIP_BYTES(67 - lmd_count);

        return true;
    }
}
}
