// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_SPAN_UNDERLINE_SPAN_H_
#define UKIVE_TEXT_SPAN_UNDERLINE_SPAN_H_

#include <optional>

#include "ukive/graphics/color.h"
#include "ukive/text/span/span.h"


namespace ukive {

    class TextCustomDrawing;

    class DrawingEffectsSpan : public Span {
    public:
        DrawingEffectsSpan(size_t start, size_t end);

        Type getType() const override { return DRAWING_EFFECTS; }

        bool has_underline = false;
        bool has_strikethrough = false;
        bool has_custom_draw = false;

        std::optional<Color> text_color;
        std::optional<Color> underline_color;
        std::optional<Color> strikethrough_color;
        TextCustomDrawing* tcd;
    };

}

#endif  // UKIVE_TEXT_SPAN_UNDERLINE_SPAN_H_