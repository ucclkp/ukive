// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_MAC_TEXT_LAYOUT_MAC_H_
#define UKIVE_TEXT_MAC_TEXT_LAYOUT_MAC_H_

#include "ukive/text/text_layout.h"

#include "utils/mac/objc_utils.hpp"

UTL_OBJC_CLASS(NSLayoutManager);
UTL_OBJC_CLASS(NSTextStorage);
UTL_OBJC_CLASS(NSTextContainer);


namespace ukive {

    class Color;

    class TextLayoutMac : public TextLayout {
    public:
        TextLayoutMac();

        bool make(
            const std::u16string &text,
            const std::u16string &font_name,
            float font_size,
            TextLayout::FontStyle style,
            TextLayout::FontWeight weight,
            const std::u16string &locale_name) override;
        void destroy() override;

        void setMaxWidth(int max_width) override;
        void setMaxHeight(int max_height) override;

        void setFontAttributes(const FontAttributes &attrs, const Range &range) override;
        void setDrawingEffects(const DrawingEffects &effects, const Range &range) override;
        void setInlineObject(TextInlineObject *tio, const Range &range) override;

        void setDefaultFontColor(const Color &color) override;

        void setTextAlignment(Alignment align) override;
        void setParagraphAlignment(Alignment align) override;
        void setTextWrapping(TextWrapping tw) override;
        void setLineSpacing(LineSpacing ls, float spacing) override;

        int getMaxWidth() const override;
        int getMaxHeight() const override;

        bool getTextMetrics(TextMetrics *tm) override;
        bool getLineMetrics(std::vector<LineMetrics> *lms) override;

        bool hitTestPoint(
            float x, float y, bool *is_trailing, bool *is_inside, HitTestInfo *info) override;
        bool hitTestTextRange(
            size_t pos, size_t len, float org_x, float org_y, std::vector<HitTestInfo> *info) override;
        bool hitTestTextPos(
            size_t pos, bool is_trailing, PointF *pt, HitTestInfo *info) override;

        void draw(Canvas* c, float x, float y) override;

        void nativeDraw(float x, float y);

    private:
        void calculateGlyphRange();

        Range glyph_range_;
        bool glyph_range_cached_ = false;

        NSTextStorage* text_storage_ = nullptr;
        NSLayoutManager* layout_mgr_ = nullptr;
        NSTextContainer* text_container_ = nullptr;

        float def_font_size_ = 0;
        Alignment vert_align_ = Alignment::LEADING;
    };

}

#endif  // UKIVE_TEXT_MAC_TEXT_LAYOUT_MAC_H_
