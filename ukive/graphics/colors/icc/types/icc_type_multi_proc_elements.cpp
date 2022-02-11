// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "icc_type_multi_proc_elements.h"

#include <cassert>

#include "utils/memory/aligments.hpp"
#include "utils/stream_utils.h"

#define ELEMENT_PARSER(ele_sign, parser, ele_class)  \
    case ele_sign: {                    \
        auto ele = new ele_class();     \
        ele->signature = ele_sign;      \
        if (parser(s, ele_size, ele)) { \
            elements.push_back(ele);    \
        } else {              \
            assert(false);    \
            delete ele;       \
        }                     \
        break;                \
    }

#define CURVE_PARSER(crv_sign, parser, crv_class)  \
    case crv_sign: {                     \
        crv_class crv;                   \
        crv.signature = crv_sign;        \
        if (parser(s, crv_size, &crv)) { \
            element->curves.push_back(std::move(crv));  \
        } else {              \
            assert(false);    \
        }                     \
        break;                \
    }


namespace ukive {
namespace icc {

    MultiProcElementsType::MultiProcElementsType(uint32_t type)
        : ICCType(type)
    {}

    MultiProcElementsType::~MultiProcElementsType() {
        utl::STLDeleteElements(&elements);
    }

    bool MultiProcElementsType::onParse(std::istream& s, uint32_t size) {
        auto start_pos = s.tellg();
        SKIP_BYTES(4);

        uint16_t element_num;
        READ_STREAM_BE(in_num, 2);
        READ_STREAM_BE(out_num, 2);
        READ_STREAM_BE(element_num, 2);
        if (element_num == 0) {
            return false;
        }

        struct Position {
            uint32_t offset;
            uint32_t size;
        };

        std::vector<Position> pos_table;
        pos_table.resize(element_num);

        for (uint16_t i = 0; i < element_num; ++i) {
            READ_STREAM_BE(pos_table[i].offset, 4);
            READ_STREAM_BE(pos_table[i].size, 4);
        }

        for (uint16_t i = 0; i < element_num; ++i) {
            auto offset = pos_table[i].offset;
            auto ele_size = pos_table[i].size;
            SEEKG_STREAM(start_pos + std::streamoff(offset - 4));

            uint32_t ele_sign;
            READ_STREAM_BE(ele_sign, 4);

            switch (ele_sign) {
                ELEMENT_PARSER(kElementCurveSet, parseElementCurveSet, CurveSetElement);
                ELEMENT_PARSER(kElementMatrix,   parseElementMatrix,   MatrixElement);
                ELEMENT_PARSER(kElementCLUT,     parseElementCLUT,     ClutElement);
                ELEMENT_PARSER(kElementFuture1,  parseElementFuture,   FutureElement);
                ELEMENT_PARSER(kElementFuture2,  parseElementFuture,   FutureElement);

            default:
                assert(false);
                return false;
            }
        }

        return true;
    }

    bool MultiProcElementsType::onCheckSize(uint32_t read_size, uint32_t total_size) const {
        return read_size <= total_size;
    }

    // static
    bool MultiProcElementsType::parseElementCurveSet(
        std::istream& s, uint32_t size, CurveSetElement* element)
    {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);
        READ_STREAM_BE(element->in_num, 2);
        READ_STREAM_BE(element->out_num, 2);

        struct Position {
            uint32_t offset;
            uint32_t size;
        };

        std::vector<Position> pos_table;
        pos_table.resize(element->out_num);

        for (uint16_t i = 0; i < element->out_num; ++i) {
            READ_STREAM_BE(pos_table[i].offset, 4);
            READ_STREAM_BE(pos_table[i].size, 4);
        }

        for (uint16_t i = 0; i < element->out_num; ++i) {
            auto offset = pos_table[i].offset;
            auto crv_size = pos_table[i].size;
            SEEKG_STREAM(start_pos + std::streamoff(offset - 4));

            uint32_t crv_sign;
            READ_STREAM_BE(crv_sign, 4);

            switch (crv_sign) {
            case kCurveMulti:
            {
                CurveSetElement::Curve crv;
                if (parseCurve(s, crv_size, &crv)) {
                    element->curves.push_back(std::move(crv));
                } else {
                    assert(false);
                }
                break;
            }

            case kSegmentFormula:
            {
                CurveSetElement::Curve crv;
                auto seg = new CurveSetElement::FormulaSegment();
                seg->signature = crv_sign;
                if (parseSegmentFormula(s, crv_size, seg)) {
                    crv.segments.push_back(seg);
                    element->curves.push_back(std::move(crv));
                } else {
                    delete seg;
                    assert(false);
                }
                break;
            }

            case kSegmentSampled:
            {
                CurveSetElement::Curve crv;
                auto seg = new CurveSetElement::SampledSegment();
                seg->signature = crv_sign;
                if (parseSegmentSampled(s, crv_size, seg)) {
                    crv.segments.push_back(seg);
                    element->curves.push_back(std::move(crv));
                } else {
                    delete seg;
                    assert(false);
                }
                break;
            }

            default:
                assert(false);
                return false;
            }
        }

        if (s.tellg() - start_pos + 4 > size) {
            return false;
        }

        return true;
    }

    // static
    bool MultiProcElementsType::parseElementMatrix(
        std::istream& s, uint32_t size, MatrixElement* element)
    {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);
        READ_STREAM_BE(element->in_num, 2);
        READ_STREAM_BE(element->out_num, 2);

        uint32_t count = element->in_num * element->out_num;
        element->matrix.resize(count);
        for (uint32_t i = 0; i < count; ++i) {
            READ_STREAM_BE(element->matrix[i], 4);
        }

        if (s.tellg() - start_pos + 4 > size) {
            return false;
        }

        return true;
    }

    // static
    bool MultiProcElementsType::parseElementCLUT(
        std::istream& s, uint32_t size, ClutElement* element)
    {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);
        READ_STREAM_BE(element->in_num, 2);
        READ_STREAM_BE(element->out_num, 2);

        uint8_t gp_nums[16];
        if (element->in_num > 16) {
            return false;
        }
        READ_STREAM(gp_nums, element->in_num);
        SKIP_BYTES(16Ui8 - element->in_num);

        size_t element_size = element->out_num;
        for (uint8_t i = 0; i < element->in_num; ++i) {
            element_size *= gp_nums[i];
        }

        element->clut.resize(element_size);
        for (size_t i = 0; i < element_size; ++i) {
            READ_STREAM_BE(element->clut[i], 4);
        }

        if (s.tellg() - start_pos + 4 > size) {
            return false;
        }

        return true;
    }

    // static
    bool MultiProcElementsType::parseElementFuture(
        std::istream& s, uint32_t size, FutureElement* element)
    {
        auto start_pos = s.tellg();

        SKIP_BYTES(4);
        READ_STREAM_BE(element->in_num, 2);
        READ_STREAM_BE(element->out_num, 2);
        READ_STREAM_BE(element->ex_signature, 4);

        if (element->in_num != element->out_num) {
            return false;
        }

        if (s.tellg() - start_pos + 4 > size) {
            return false;
        }

        return true;
    }

    // static
    bool MultiProcElementsType::parseCurve(
        std::istream& s, uint32_t size, CurveSetElement::Curve* curve)
    {
        auto start_pos = s.tellg();
        SKIP_BYTES(4);

        uint16_t seg_num;
        READ_STREAM_BE(seg_num, 2);
        if (seg_num == 0) {
            return false;
        }

        SKIP_BYTES(2);

        curve->break_points.resize(seg_num - 1);
        for (uint16_t i = 0; i < seg_num - 1; ++i) {
            READ_STREAM_BE(curve->break_points[i], 4);
        }

        for (uint16_t i = 0; i < seg_num - 1; ++i) {
            SKIP_BYTES(utl::align4off(uint32_t(s.tellg())));

            uint32_t seg_sign;
            READ_STREAM_BE(seg_sign, 4);

            switch (seg_sign) {
            case kSegmentFormula:
            {
                auto seg = new CurveSetElement::FormulaSegment();
                seg->signature = seg_sign;
                if (parseSegmentFormula(s, 0, seg)) {
                    curve->segments.push_back(seg);
                } else {
                    delete seg;
                    assert(false);
                }
                break;
            }

            case kSegmentSampled:
            {
                CurveSetElement::Curve crv;
                auto seg = new CurveSetElement::SampledSegment();
                seg->signature = seg_sign;
                if (parseSegmentSampled(s, 0, seg)) {
                    curve->segments.push_back(seg);
                } else {
                    delete seg;
                    assert(false);
                }
                break;
            }

            default:
                assert(false);
                return false;
            }
        }

        if (s.tellg() - start_pos + 4 > size) {
            return false;
        }
        return true;
    }

    // static
    bool MultiProcElementsType::parseSegmentFormula(
        std::istream& s, uint32_t size, CurveSetElement::FormulaSegment* seg)
    {
        SKIP_BYTES(4);

        READ_STREAM_BE(seg->func_type, 2);
        SKIP_BYTES(2);

        switch (seg->func_type) {
        case 0x0000:
            READ_STREAM_BE(seg->y, 4);
            READ_STREAM_BE(seg->a, 4);
            READ_STREAM_BE(seg->b, 4);
            READ_STREAM_BE(seg->c, 4);
            break;

        case 0x0001:
            READ_STREAM_BE(seg->y, 4);
            READ_STREAM_BE(seg->a, 4);
            READ_STREAM_BE(seg->b, 4);
            READ_STREAM_BE(seg->c, 4);
            READ_STREAM_BE(seg->d, 4);
            break;

        case 0x0002:
            READ_STREAM_BE(seg->a, 4);
            READ_STREAM_BE(seg->b, 4);
            READ_STREAM_BE(seg->c, 4);
            READ_STREAM_BE(seg->d, 4);
            READ_STREAM_BE(seg->e, 4);
            break;

        default:
            assert(false);
            return false;
        }

        return true;
    }

    // static
    bool MultiProcElementsType::parseSegmentSampled(
        std::istream& s, uint32_t size, CurveSetElement::SampledSegment* seg)
    {
        SKIP_BYTES(4);

        uint32_t ent_num;
        READ_STREAM_BE(ent_num, 4);
        seg->entries.resize(ent_num);

        for (uint32_t i = 0; i < ent_num; ++i) {
            READ_STREAM_BE(seg->entries[i], 4);
        }

        return true;
    }

}
}
