// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_TEXT_DESC_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_TEXT_DESC_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <istream>


namespace ukive {
namespace icc {

    class TextDescType : public ICCType {
    public:
        explicit TextDescType(uint32_t type);

        std::string ascii_text;
        uint32_t unicode_lang_code = 0;
        std::u16string unicode_loc_desc;
        uint16_t script_code = 0;
        std::string loc_mac_desc;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;

    private:

    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_TEXT_DESC_H_