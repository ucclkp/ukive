// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_SPAN_TEXT_ATTRIBUTES_SPAN_H_
#define UKIVE_TEXT_SPAN_TEXT_ATTRIBUTES_SPAN_H_

#include <optional>

#include "ukive/text/span/span.h"
#include "ukive/text/text_layout.h"


namespace ukive {

    class TextAttributesSpan : public Span {
    public:
        TextAttributesSpan(size_t start, size_t end);

        Type getType() const override { return FONT_ATTRIBUTES; }

        std::optional<int> font_size;
        std::optional<TextLayout::FontStyle> font_style;
        std::optional<TextLayout::FontWeight> font_weight;
        std::u16string font_name;
    };

}

#endif  // UKIVE_TEXT_SPAN_TEXT_ATTRIBUTES_SPAN_H_