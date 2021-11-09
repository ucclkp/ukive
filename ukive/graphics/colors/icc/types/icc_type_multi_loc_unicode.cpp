// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_multi_loc_unicode.h"

#include "utils/stream_utils.h"


namespace ukive {
namespace icc {

    MultiLocUnicodeType::MultiLocUnicodeType(uint32_t type)
        : ICCType(type) {}

    bool MultiLocUnicodeType::onParse(std::istream& s, uint32_t size) {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);

        uint32_t count;
        READ_STREAM_BE(count, 4);
        records.resize(count);

        for (uint32_t i = 0; i < count; ++i) {
            Record record;
            READ_STREAM_BE(record.lang_code, 2);
            READ_STREAM_BE(record.country_code, 2);

            uint32_t str_len;
            READ_STREAM_BE(str_len, 4);
            record.str.resize(str_len);

            uint32_t offset;
            READ_STREAM_BE(offset, 4);

            if (str_len > 0) {
                auto saved_pos = s.tellg();
                SEEKG_STREAM(start_pos + std::streamoff(offset - 4));
                for (uint32_t j = 0; j < str_len; ++j) {
                    READ_STREAM_BE(record.str[j], 2);
                }
                SEEKG_STREAM(saved_pos);
            }

            records[i] = std::move(record);
        }

        return true;
    }

}
}
