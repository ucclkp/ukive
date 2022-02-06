// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_ICC_PARSER_H_
#define UKIVE_GRAPHICS_COLORS_ICC_ICC_PARSER_H_

#include <istream>
#include <vector>

#include "ukive/graphics/colors/icc/types/icc_type_lut16.h"


namespace ukive {
namespace icc {

    // ICC 4.3 Parser
    class ICCParser {
    public:
        struct ProfileHeader {
            uint32_t profile_size;
            uint32_t preferred_cmm_type;
            uint32_t profile_version;
            uint32_t profile_dev_class;
            uint32_t data_color_space;
            uint32_t pcs;
            DateTimeNumber create_time;
            uint32_t profile_file_signature;
            uint32_t primary_platform_signature;
            uint32_t profile_flags;
            uint32_t dev_manufacturer;
            uint32_t dev_model;
            uint64_t dev_attrs;
            uint32_t rendering_intent;
            XYZNumber pcs_illuminant_nCIEXYZ;
            uint32_t profile_creator_signature;
            uint8_t profile_id[16];
        };

        struct TagElement {
            bool valid = false;
            uint32_t tag_signature;
            uint32_t data_offset;
            uint32_t data_size;

            // 以下成员由 Tag 数据解析完毕后得到
            ICCType* type = nullptr;
            uint32_t type_signature;
        };

        ICCParser();
        ~ICCParser();

        bool parseHeader(std::istream& s);
        int obtainTagData(
            std::istream& s, uint32_t tag_sign, const ICCType** type);

        const ProfileHeader& getHeader() const { return header_; }

    private:
        bool parserHeader(std::istream& s);
        bool parseTagTable(std::istream& s);
        bool parseTagData(std::istream& s, TagElement* tag);
        TagElement* getTagElement(uint32_t tag_sign);

        static size_t getTagIndex(uint32_t tag_sign);

        ProfileHeader header_;
        std::vector<TagElement> tags_;
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_ICC_PARSER_H_