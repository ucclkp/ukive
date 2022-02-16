// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_LAYOUT_H_
#define UKIVE_TEXT_TEXT_LAYOUT_H_

#include <optional>
#include <string>
#include <vector>

#include "ukive/graphics/colors/color.h"
#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"
#include "ukive/text/range.hpp"


namespace ukive {

    class Canvas;
    class TextCustomDrawing;
    class TextInlineObject;

    class TextLayout {
    public:
        static TextLayout* create();

        enum class Alignment {
            LEADING,
            CENTER,
            TRAILING,
        };

        enum class FontStyle {
            NORMAL,
            ITALIC,
        };

        enum class FontWeight {
            THIN,
            NORMAL,
            BOLD,
        };

        enum class TextWrapping {
            NONE,
            WRAP,
        };

        enum class LineSpacing {
            DEFAULT,
            UNIFORM,
            PROPORTIONAL,
        };

        struct FontAttributes {
            bool is_removed;
            std::optional<int> size;
            std::optional<FontStyle> style;
            std::optional<FontWeight> weight;
            std::u16string name;
        };

        struct DrawingEffects {
            bool is_removed;
            bool has_underline;
            bool has_strikethrough;
            bool has_custom_draw;
            std::optional<Color> text_color;
            std::optional<Color> underline_color;
            std::optional<Color> strikethrough_color;
            TextCustomDrawing* tcd;
        };

        struct TextMetrics {
            RectF rect;
            size_t line_count;
        };

        struct LineMetrics {
            size_t length;
            float height;
            float baseline;
        };

        struct HitTestInfo {
            size_t pos;
            size_t length;
            RectF rect;
        };

        virtual ~TextLayout() = default;

        virtual bool make(
            const std::u16string_view& text,
            const std::u16string_view& font_name,
            float font_size,
            FontStyle style,
            FontWeight weight,
            const std::u16string_view& locale_name) = 0;
        virtual void destroy() = 0;

        virtual void setMaxWidth(float max_width) = 0;
        virtual void setMaxHeight(float max_height) = 0;

        virtual void setFontAttributes(const FontAttributes& attrs, const Range& range) = 0;
        virtual void setDrawingEffects(const DrawingEffects& effects, const Range& range) = 0;
        virtual void setInlineObject(TextInlineObject* tio, const Range& range) = 0;

        virtual void setDefaultFontColor(const Color& color) = 0;

        virtual void setTextAlignment(Alignment align) = 0;
        virtual void setParagraphAlignment(Alignment align) = 0;
        virtual void setTextWrapping(TextWrapping tw) = 0;
        virtual void setLineSpacing(LineSpacing ls, float spacing) = 0;

        virtual float getMaxWidth() const = 0;
        virtual float getMaxHeight() const = 0;

        virtual bool getTextMetrics(TextMetrics* tm) = 0;
        virtual bool getLineMetrics(std::vector<LineMetrics>* lms) = 0;

        virtual bool hitTestPoint(
            float x, float y, bool* is_trailing, bool* is_inside, HitTestInfo* info) = 0;
        virtual bool hitTestTextRange(
            size_t pos, size_t len, float org_x, float org_y, std::vector<HitTestInfo>* info) = 0;
        virtual bool hitTestTextPos(
            size_t pos, bool is_trailing, PointF* pt, HitTestInfo* info) = 0;

        virtual void draw(Canvas* c, float x, float y) = 0;
    };

}

#endif  // UKIVE_TEXT_TEXT_LAYOUT_H_
