// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_parser.h"

#include <cassert>

#include "utils/stream_utils.h"

#include "ukive/graphics/colors/icc/icc_constants.h"
#include "ukive/graphics/colors/icc/types/icc_type_multi_loc_unicode.h"
#include "ukive/graphics/colors/icc/types/icc_type_text_desc.h"
#include "ukive/graphics/colors/icc/types/icc_type_xyz.h"
#include "ukive/graphics/colors/icc/types/icc_type_lut8.h"
#include "ukive/graphics/colors/icc/types/icc_type_lut16.h"
#include "ukive/graphics/colors/icc/types/icc_type_curve.h"
#include "ukive/graphics/colors/icc/types/icc_type_param_curve.h"

#define TYPE_PARSER(type_sign, type_class)      \
    case type_sign: {                           \
        auto type = new type_class(type_sign);  \
        if (type->parse(s, tag->data_size)) {   \
            tag->type = type;  \
        } else {               \
            assert(false);     \
            delete type;       \
        }                      \
        break;                 \
    }


namespace ukive {
namespace icc {

    ICCParser::ICCParser() {}

    bool ICCParser::parse(std::istream& s) {
        if (!parserHeader(s)) {
            return false;
        }
        if (!parseTagTable(s)) {
            return false;
        }

        auto tag = getTagElement(kTagAToB0);
        if (tag) {
            SEEKG_STREAM(tag->data_offset);

            uint32_t type_sign;
            READ_STREAM_BE(type_sign, 4);

            switch (type_sign) {
            case kTypeLut8:
                break;

            case kTypeLut16:
                break;

            case kTypeLutAToB:
                break;

            default:
                return false;
            }
        }

        tag = getTagElement(kTagProfileDesc);
        if (tag) {
            SEEKG_STREAM(tag->data_offset);

            uint32_t type_sign;
            READ_STREAM_BE(type_sign, 4);

            switch (type_sign) {
            case kTypeMultLocUnicode:
            {
                MultiLocUnicodeType mlu_type(type_sign);
                mlu_type.parse(s, tag->data_size);
                break;
            }

            case kTypeTextDesc:
            {
                TextDescType text_desc_type(type_sign);
                text_desc_type.parse(s, tag->data_size);
                break;
            }

            default:
                return false;
            }
        }

        tag = getTagElement(kTagRedMatCol);
        if (tag) {
            SEEKG_STREAM(tag->data_offset);

            uint32_t type_sign;
            READ_STREAM_BE(type_sign, 4);

            switch (type_sign) {
            case kTypeXYZ:
                break;

            default:
                return false;
            }
        }

        return true;
    }

    const ICCType* ICCParser::parseTagData(std::istream& s, uint32_t tag_sign) {
        auto tag = getTagElement(tag_sign);
        if (!tag) {
            return nullptr;
        }

        if (tag->type) {
            return tag->type;
        }

        SEEKG_STREAM(tag->data_offset);

        uint32_t type_sign;
        READ_STREAM_BE(type_sign, 4);

        tag->type_signature = type_sign;

        switch (type_sign) {
            TYPE_PARSER(kTypeCurve, CurveType);
            TYPE_PARSER(kTypeLut16, Lut16Type);
            TYPE_PARSER(kTypeLut8, Lut8Type);
            TYPE_PARSER(kTypeParamCurve, ParamCurveType);
            //TYPE_PARSER(kTypeLutAToB, );
            TYPE_PARSER(kTypeMultLocUnicode, MultiLocUnicodeType);
            TYPE_PARSER(kTypeTextDesc, TextDescType);
            TYPE_PARSER(kTypeXYZ, XYZType);

        default:
            assert(false);
            break;
        }

        return tag->type;
    }

    bool ICCParser::parserHeader(std::istream& s) {
        auto header = &header_;

        READ_STREAM_BE(header->profile_size, 4);
        READ_STREAM_BE(header->preferred_cmm_type, 4);
        READ_STREAM_BE(header->profile_version, 4);
        READ_STREAM_BE(header->profile_dev_class, 4);
        READ_STREAM_BE(header->data_color_space, 4);
        READ_STREAM_BE(header->pcs, 4);
        READ_STREAM_BE(header->create_time.year, 2);
        READ_STREAM_BE(header->create_time.month, 2);
        READ_STREAM_BE(header->create_time.day, 2);
        READ_STREAM_BE(header->create_time.hours, 2);
        READ_STREAM_BE(header->create_time.minutes, 2);
        READ_STREAM_BE(header->create_time.seconds, 2);

        READ_STREAM_BE(header->profile_file_signature, 4);
        if (header->profile_file_signature != kFileSignature) {
            return false;
        }

        READ_STREAM_BE(header->primary_platform_signature, 4);
        READ_STREAM_BE(header->profile_flags, 4);
        READ_STREAM_BE(header->dev_manufacturer, 4);
        READ_STREAM_BE(header->dev_model, 4);
        READ_STREAM_BE(header->dev_attrs, 8);
        READ_STREAM_BE(header->rendering_intent, 4);
        READ_STREAM_BE(header->pcs_illuminant_nCIEXYZ.x, 4);
        READ_STREAM_BE(header->pcs_illuminant_nCIEXYZ.y, 4);
        READ_STREAM_BE(header->pcs_illuminant_nCIEXYZ.z, 4);
        READ_STREAM_BE(header->profile_creator_signature, 4);
        READ_STREAM(header->profile_id[0], 16);
        SKIP_BYTES(28);
        return true;
    }

    bool ICCParser::parseTagTable(std::istream& s) {
        uint32_t count;
        READ_STREAM_BE(count, 4);

        tags_.resize(63);
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t sign;
            READ_STREAM_BE(sign, 4);

            size_t index = getTagIndex(sign);
            if (index == size_t(-1)) {
                TagElement tag;
                tag.valid = true;
                tag.tag_signature = sign;
                READ_STREAM_BE(tag.data_offset, 4);
                READ_STREAM_BE(tag.data_size, 4);
                tags_.push_back(tag);
            } else {
                tags_[index].valid = true;
                tags_[index].tag_signature = sign;
                READ_STREAM_BE(tags_[index].data_offset, 4);
                READ_STREAM_BE(tags_[index].data_size, 4);
            }
        }

        return true;
    }

    ICCParser::TagElement* ICCParser::getTagElement(uint32_t tag_sign) {
        size_t index = getTagIndex(tag_sign);
        if (index == size_t(-1)) {
            return nullptr;
        }

        auto ptr = &tags_[index];
        if (!ptr->valid) {
            return nullptr;
        }

        if (ptr->tag_signature != tag_sign) {
            assert(false);
            return nullptr;
        }

        return ptr;
    }

    // static
    size_t ICCParser::getTagIndex(uint32_t tag_sign) {
        switch (tag_sign) {
        case kTagAToB0:      return 0;
        case kTagAToB1:      return 1;
        case kTagAToB2:      return 2;
        case kTagBlueMatCol: return 3;
        case kTagBlueTRC:    return 4;
        case kTagBToA0:      return 5;
        case kTagBToA1:      return 6;
        case kTagBToA2:      return 7;
        case kTagBToD0:      return 8;
        case kTagBToD1:      return 9;
        case kTagBToD2:      return 10;
        case kTagBToD3:      return 11;
        case kTagCalibDateTime:  return 12;
        case kTagCharTarget:     return 13;
        case kTagChromAdapt:     return 14;
        case kTagChromaticity:   return 15;
        case kTagColorOrder:     return 16;
        case kTagColorTable:     return 17;
        case kTagColorTableOut:  return 18;
        case kTagClrIntImgState: return 19;
        case kTagCopyright:      return 20;
        case kTagCrdInfo:        return 21;
        case kTagDevMfgDesc:     return 22;
        case kTagDevModelDesc:   return 23;
        case kTagDevSettings:    return 24;
        case kTagDToB0:          return 25;
        case kTagDToB1:          return 26;
        case kTagDToB2:          return 27;
        case kTagDToB3:          return 28;
        case kTagGamut:          return 29;
        case kTagGrayTRC:        return 30;
        case kTagGreenMatCol:    return 31;
        case kTagGreenTRC:       return 32;
        case kTagICC5:           return 33;
        case kTagLuminance:      return 34;
        case kTagMeasure:        return 35;
        case kTagMediaBlackPoint:   return 36;
        case kTagMediaWhitePoint:   return 37;
        case kTagNamedColor:        return 38;
        case kTagNamedColor2:       return 39;
        case kTagOutputResp:        return 40;
        case kTagPerRenderIntGamut: return 41;
        case kTagPreview0:          return 42;
        case kTagPreview1:          return 43;
        case kTagPreview2:          return 44;
        case kTagProfileDesc:       return 45;
        case kTagProfileSeqDesc:    return 46;
        case kTagProfileSeqId:      return 47;
        case kTagPS2CRD0:           return 48;
        case kTagPS2CRD1:           return 49;
        case kTagPS2CRD2:           return 50;
        case kTagPS2CRD3:           return 51;
        case kTagPS2CSA:            return 52;
        case kTagPS2RenderIntent:   return 53;
        case kTagRedMatCol:         return 54;
        case kTagRedTRC:            return 55;
        case kTagScreeningDesc:     return 56;
        case kTagScreening:         return 57;
        case kTagSatRenderIntGamut: return 58;
        case kTagTechnology:        return 59;
        case kTagUcrbg:             return 60;
        case kTagViewCondDesc:      return 61;
        case kTagViewConds:         return 62;
        default: return -1;
        }
    }

}
}
