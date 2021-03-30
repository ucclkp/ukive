// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_DW_TEXT_LAYOUT_H_
#define UKIVE_TEXT_WIN_DW_TEXT_LAYOUT_H_

#include <functional>

#include <dwrite.h>

#include "ukive/system/win/com_ptr.hpp"
#include "ukive/text/text_layout.h"


namespace ukive {

    class TextRenderer;
    class DWTextDrawingEffect;

    class DWTextLayout : public TextLayout {
    public:
        DWTextLayout();

        bool make(
            const std::u16string& text,
            const std::u16string& font_name,
            float font_size,
            FontStyle style,
            FontWeight weight,
            const std::u16string& locale_name) override;
        void destroy() override;

        void setMaxWidth(int max_width) override;
        void setMaxHeight(int max_height) override;

        void setFontAttributes(const FontAttributes& attrs, const Range& range) override;
        void setDrawingEffects(const DrawingEffects& effects, const Range& range) override;
        void setInlineObject(TextInlineObject* tio, const Range& range) override;

        void setDefaultFontColor(const Color& color) override;

        void setTextAlignment(Alignment align) override;
        void setParagraphAlignment(Alignment align) override;
        void setTextWrapping(TextWrapping tw) override;
        void setLineSpacing(LineSpacing ls, float spacing) override;

        int getMaxWidth() const override;
        int getMaxHeight() const override;

        bool getTextMetrics(TextMetrics* tm) override;
        bool getLineMetrics(std::vector<LineMetrics>* lms) override;

        bool hitTestPoint(
            float x, float y, bool* is_trailing, bool* is_inside, HitTestInfo* info) override;
        bool hitTestTextRange(
            size_t pos, size_t len, float org_x, float org_y, std::vector<HitTestInfo>* info) override;
        bool hitTestTextPos(
            size_t pos, bool is_trailing, PointF* pt, HitTestInfo* info) override;

        void draw(Canvas* c, float x, float y) override;

        IDWriteTextLayout* getNative() const { return text_layout_.get(); }

    private:
        float getMaxLineHeight();

        uint32_t length_ = 0;
        float line_spacing_ = 0;
        LineSpacing line_spacing_method_ = LineSpacing::DEFAULT;

        ComPtr<TextRenderer> text_renderer_;
        ComPtr<IDWriteTextFormat> text_format_;
        ComPtr<IDWriteTextLayout> text_layout_;
    };

}

#endif  // UKIVE_TEXT_WIN_DW_TEXT_LAYOUT_H_