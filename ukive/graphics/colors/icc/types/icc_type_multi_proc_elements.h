// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_MULTI_PROC_ELEMENTS_H_
#define UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_MULTI_PROC_ELEMENTS_H_

#include "ukive/graphics/colors/icc/types/icc_type.h"

#include <istream>
#include <vector>

#include "utils/stl_utils.h"


namespace ukive {
namespace icc {

    constexpr uint32_t kElementCurveSet = 0x6D666C74; // 'cvst'
    constexpr uint32_t kElementMatrix   = 0x6D617466; // 'matf'
    constexpr uint32_t kElementCLUT     = 0x636C7574; // 'clut'
    constexpr uint32_t kElementFuture1  = 0x62414353; // 'bACS'
    constexpr uint32_t kElementFuture2  = 0x65414353; // 'eACS'

    constexpr uint32_t kCurveMulti     = 0x63757266; // 'curf'
    constexpr uint32_t kSegmentFormula = 0x70617266; // 'parf'
    constexpr uint32_t kSegmentSampled = 0x73616D66; // 'samf'


    class MultiProcElementsType : public ICCType {
    public:
        struct Element {
            uint32_t signature;
            uint16_t in_num;
            uint16_t out_num;
        };

        struct CurveSetElement : Element {
            struct Segment {
                uint32_t signature;
            };

            struct FormulaSegment : Segment {
                uint16_t func_type;
                float y, a, b, c, d, e;
            };

            struct SampledSegment : Segment {
                std::vector<float> entries;
            };

            struct Curve {
                ~Curve() {
                    utl::STLDeleteElements(&segments);
                }

                std::vector<Segment*> segments;
                std::vector<float> break_points;
            };

            uint16_t in_num;
            uint16_t out_num;
            std::vector<Curve> curves;
        };

        struct MatrixElement : Element {
            uint16_t in_num;
            uint16_t out_num;
            std::vector<float> matrix;
        };

        struct ClutElement : Element {
            uint16_t in_num;
            uint16_t out_num;
            uint8_t gp_num[16];
            std::vector<float> clut;
        };

        struct FutureElement : Element {
            uint16_t in_num;
            uint16_t out_num;
            uint32_t ex_signature;
        };

        explicit MultiProcElementsType(uint32_t type);
        ~MultiProcElementsType();

        uint16_t in_num = 0;
        uint16_t out_num = 0;
        std::vector<Element*> elements;

    protected:
        bool onParse(std::istream& s, uint32_t size) override;
        bool onCheckSize(uint32_t read_size, uint32_t total_size) const override;

        static bool parseElementCurveSet(
            std::istream& s, uint32_t size, CurveSetElement* element);
        static bool parseElementMatrix(
            std::istream& s, uint32_t size, MatrixElement* element);
        static bool parseElementCLUT(
            std::istream& s, uint32_t size, ClutElement* element);
        static bool parseElementFuture(
            std::istream& s, uint32_t size, FutureElement* element);

        static bool parseCurve(
            std::istream& s, uint32_t size, CurveSetElement::Curve* curve);
        static bool parseSegmentFormula(
            std::istream& s, uint32_t size, CurveSetElement::FormulaSegment* seg);
        static bool parseSegmentSampled(
            std::istream& s, uint32_t size, CurveSetElement::SampledSegment* seg);
    };

}
}

#endif  // UKIVE_GRAPHICS_COLORS_ICC_TYPES_ICC_TYPE_MULTI_PROC_ELEMENTS_H_