// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_MULTI_LOC_UNICODE_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_MULTI_LOC_UNICODE_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <vector>


namespace ukive {
namespace icc {

    class MultiLocUnicodeType : public ICCType {
    public:
        struct Record {
            uint16_t lang_code;
            uint16_t country_code;
            std::u16string str;
        };

        explicit MultiLocUnicodeType(uint32_t type);

        std::vector<Record> records;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;

    private:

    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_MULTI_LOC_UNICODE_H_