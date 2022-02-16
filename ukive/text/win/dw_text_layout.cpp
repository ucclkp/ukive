// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/win/dw_text_layout.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/win/directx_manager.h"
#include "ukive/text/win/dw_inline_object.h"
#include "ukive/text/win/dw_text_drawing_effect.h"
#include "ukive/text/win/dw_text_renderer.h"

#include <VersionHelpers.h>


namespace ukive {

    DWRITE_FONT_STYLE mapFontStyle(TextLayout::FontStyle s) {
        DWRITE_FONT_STYLE style;
        switch (s) {
        case TextLayout::FontStyle::NORMAL: style = DWRITE_FONT_STYLE_NORMAL; break;
        case TextLayout::FontStyle::ITALIC: style = DWRITE_FONT_STYLE_ITALIC; break;
        default: style = DWRITE_FONT_STYLE_NORMAL; break;
        }
        return style;
    }

    DWRITE_FONT_WEIGHT mapFontWeight(TextLayout::FontWeight w) {
        DWRITE_FONT_WEIGHT weight;
        switch (w) {
        case TextLayout::FontWeight::THIN: weight = DWRITE_FONT_WEIGHT_THIN; break;
        case TextLayout::FontWeight::NORMAL: weight = DWRITE_FONT_WEIGHT_NORMAL; break;
        case TextLayout::FontWeight::BOLD: weight = DWRITE_FONT_WEIGHT_BOLD; break;
        default: weight = DWRITE_FONT_WEIGHT_NORMAL; break;
        }
        return weight;
    }

    DWRITE_TEXT_RANGE makeTextRange(const Range& range) {
        return { UINT32(range.pos), UINT32(range.length) };
    }

    bool rangeIntersect(const Range& r1, const DWRITE_TEXT_RANGE& r2) {
        auto _pos = std::max<size_t>(r1.pos, r2.startPosition);
        auto _end = std::min<size_t>(r1.end(), r2.startPosition + r2.length);
        return _end > _pos;
    }

}

namespace ukive {
namespace win {

    DWTextLayout::DWTextLayout() {
        text_renderer_ = new TextRenderer();
    }

    bool DWTextLayout::make(
        const std::u16string_view& text,
        const std::u16string_view& font_name,
        float font_size,
        FontStyle style,
        FontWeight weight,
        const std::u16string_view& locale_name)
    {
        auto dwrite_factory = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getDWriteFactory();

        std::wstring w_fn(font_name.begin(), font_name.end());
        std::wstring w_ln(locale_name.begin(), locale_name.end());
        std::wstring w_text(text.begin(), text.end());

        text_format_.reset();
        HRESULT hr = dwrite_factory->CreateTextFormat(
            w_fn.c_str(), nullptr,
            mapFontWeight(weight),
            mapFontStyle(style),
            DWRITE_FONT_STRETCH_NORMAL,
            font_size,
            w_ln.c_str(),
            &text_format_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create text format: " << hr;
            return false;
        }

        length_ = utl::num_cast<uint32_t>(w_text.length());

        text_layout_.reset();
        hr = dwrite_factory->CreateTextLayout(
            w_text.c_str(), length_,
            text_format_.get(), 0, 0, &text_layout_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create text layout: " << hr;
            return false;
        }

        return true;
    }

    void DWTextLayout::destroy() {
        text_layout_.reset();
        text_format_.reset();
    }

    void DWTextLayout::setMaxWidth(float max_width) {
        HRESULT hr = text_layout_->SetMaxWidth(max_width);
        ubassert(SUCCEEDED(hr));
    }

    void DWTextLayout::setMaxHeight(float max_height) {
        HRESULT hr = text_layout_->SetMaxHeight(max_height);
        ubassert(SUCCEEDED(hr));
    }

    void DWTextLayout::setFontAttributes(const FontAttributes& attrs, const Range& range) {
        if (range.empty() ||
            range.length == 0 ||
            range.end() > length_)
        {
            return;
        }

        HRESULT hr;
        if (attrs.size.has_value()) {
            hr = text_layout_->SetFontSize(float(*attrs.size), makeTextRange(range));
            ubassert(SUCCEEDED(hr));
        }
        if (attrs.style.has_value()) {
            hr = text_layout_->SetFontStyle(mapFontStyle(*attrs.style), makeTextRange(range));
            ubassert(SUCCEEDED(hr));
        }
        if (attrs.weight.has_value()) {
            hr = text_layout_->SetFontWeight(mapFontWeight(*attrs.weight), makeTextRange(range));
            ubassert(SUCCEEDED(hr));
        }
        if (!attrs.name.empty()) {
            std::wstring w_attr(attrs.name.begin(), attrs.name.end());
            hr = text_layout_->SetFontFamilyName(w_attr.c_str(), makeTextRange(range));
            ubassert(SUCCEEDED(hr));
        }

        if (line_spacing_method_ == LineSpacing::UNIFORM) {
            setLineSpacing(line_spacing_method_, line_spacing_);
        }
    }

    void DWTextLayout::setDrawingEffects(const DrawingEffects& effects, const Range& range) {
        if (range.empty() ||
            range.length == 0 ||
            range.end() > length_)
        {
            return;
        }

        HRESULT hr = text_layout_->SetUnderline(
            effects.has_underline ? TRUE : FALSE, makeTextRange(range));
        ubassert(SUCCEEDED(hr));
        hr = text_layout_->SetStrikethrough(
            effects.has_strikethrough ? TRUE : FALSE, makeTextRange(range));
        ubassert(SUCCEEDED(hr));

        auto drawing_effect = new DWTextDrawingEffect();
        drawing_effect->text_color_enabled = effects.text_color.has_value();
        drawing_effect->alter_underline_color = effects.underline_color.has_value();
        drawing_effect->alter_strikethrough_color = effects.strikethrough_color.has_value();
        drawing_effect->custom_drawing_enabled = effects.has_custom_draw;
        drawing_effect->custom_drawing_cb = effects.tcd;
        if (drawing_effect->text_color_enabled) {
            drawing_effect->text_color = *effects.text_color;
        }
        if (drawing_effect->alter_underline_color) {
            drawing_effect->underline_color = *effects.underline_color;
        }
        if (drawing_effect->alter_strikethrough_color) {
            drawing_effect->strikethrough_color = *effects.strikethrough_color;
        }

        hr = text_layout_->SetDrawingEffect(drawing_effect, makeTextRange(range));
        ubassert(SUCCEEDED(hr));
    }

    void DWTextLayout::setInlineObject(TextInlineObject* tio, const Range& range) {
        if (range.empty() ||
            range.length == 0 ||
            range.end() > length_)
        {
            return;
        }

        DWInlineObject* inline_obj;
        if (tio) {
            inline_obj = new DWInlineObject();
            inline_obj->setCallback(tio);
        } else {
            inline_obj = nullptr;
        }

        HRESULT hr = text_layout_->SetInlineObject(inline_obj, makeTextRange(range));
        ubassert(SUCCEEDED(hr));

        if (line_spacing_method_ == LineSpacing::UNIFORM) {
            setLineSpacing(line_spacing_method_, line_spacing_);
        }
    }

    void DWTextLayout::setDefaultFontColor(const Color& color) {
        text_renderer_->setTextColor(color);
    }

    void DWTextLayout::setTextAlignment(Alignment align) {
        DWRITE_TEXT_ALIGNMENT dw_align;
        switch (align) {
        case Alignment::LEADING: dw_align = DWRITE_TEXT_ALIGNMENT_LEADING; break;
        case Alignment::CENTER: dw_align = DWRITE_TEXT_ALIGNMENT_CENTER; break;
        case Alignment::TRAILING: dw_align = DWRITE_TEXT_ALIGNMENT_TRAILING; break;
        default: dw_align = DWRITE_TEXT_ALIGNMENT_LEADING; break;
        }

        HRESULT hr = text_layout_->SetTextAlignment(dw_align);
        ubassert(SUCCEEDED(hr));
    }

    void DWTextLayout::setParagraphAlignment(Alignment align) {
        DWRITE_PARAGRAPH_ALIGNMENT dw_align;
        switch (align) {
        case Alignment::LEADING: dw_align = DWRITE_PARAGRAPH_ALIGNMENT_NEAR; break;
        case Alignment::CENTER: dw_align = DWRITE_PARAGRAPH_ALIGNMENT_CENTER; break;
        case Alignment::TRAILING: dw_align = DWRITE_PARAGRAPH_ALIGNMENT_FAR; break;
        default: dw_align = DWRITE_PARAGRAPH_ALIGNMENT_NEAR; break;
        }

        HRESULT hr = text_layout_->SetParagraphAlignment(dw_align);
        ubassert(SUCCEEDED(hr));
    }

    void DWTextLayout::setTextWrapping(TextWrapping tw) {
        DWRITE_WORD_WRAPPING wrapping;
        switch (tw) {
        case TextWrapping::NONE: wrapping = DWRITE_WORD_WRAPPING_NO_WRAP; break;
        case TextWrapping::WRAP: wrapping = DWRITE_WORD_WRAPPING_WRAP; break;
        default: wrapping = DWRITE_WORD_WRAPPING_NO_WRAP; break;
        }
        HRESULT hr = text_layout_->SetWordWrapping(wrapping);
        ubassert(SUCCEEDED(hr));
    }

    void DWTextLayout::setLineSpacing(LineSpacing ls, float spacing) {
        line_spacing_ = spacing;
        line_spacing_method_ = ls;

        DWRITE_LINE_SPACING_METHOD method;
        switch (ls) {
        case LineSpacing::DEFAULT: method = DWRITE_LINE_SPACING_METHOD_DEFAULT; break;
        case LineSpacing::UNIFORM:
            spacing += getMaxLineHeight();
            method = DWRITE_LINE_SPACING_METHOD_UNIFORM;
            break;
        case LineSpacing::PROPORTIONAL:
            if (::IsWindows10OrGreater()) {
                method = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL;
            } else {
                method = DWRITE_LINE_SPACING_METHOD_DEFAULT;
            }
            break;
        default:
            method = DWRITE_LINE_SPACING_METHOD_DEFAULT;
            break;
        }

        HRESULT hr = text_layout_->SetLineSpacing(method, spacing, spacing * 0.8f);
        ubassert(SUCCEEDED(hr));
    }

    float DWTextLayout::getMaxWidth() const {
        return text_layout_->GetMaxWidth();
    }

    float DWTextLayout::getMaxHeight() const {
        return text_layout_->GetMaxHeight();
    }

    bool DWTextLayout::getTextMetrics(TextMetrics* tm) {
        DWRITE_TEXT_METRICS metrics;
        HRESULT hr = text_layout_->GetMetrics(&metrics);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        tm->rect.left = metrics.left;
        tm->rect.top = metrics.top;
        tm->rect.right = metrics.left + metrics.widthIncludingTrailingWhitespace;
        tm->rect.bottom = metrics.top + metrics.height;
        tm->line_count = metrics.lineCount;

        return true;
    }

    bool DWTextLayout::getLineMetrics(std::vector<LineMetrics>* lms) {
        UINT32 line_count;
        DWRITE_LINE_METRICS stack[4];
        HRESULT hr = text_layout_->GetLineMetrics(stack, 4, &line_count);
        if (SUCCEEDED(hr)) {
            for (uint32_t i = 0; i < line_count; ++i) {
                LineMetrics lm;
                lm.length = stack[i].length;
                lm.height = stack[i].height;
                lm.baseline = stack[i].baseline;
                lms->push_back(std::move(lm));
            }
            return true;
        }
        if (hr == E_NOT_SUFFICIENT_BUFFER && line_count > 0) {
            std::unique_ptr<DWRITE_LINE_METRICS[]> lineMetrics(new DWRITE_LINE_METRICS[line_count]);
            hr = text_layout_->GetLineMetrics(lineMetrics.get(), line_count, &line_count);
            if (SUCCEEDED(hr)) {
                for (uint32_t i = 0; i < line_count; ++i) {
                    LineMetrics lm;
                    lm.length = lineMetrics[i].length;
                    lm.height = lineMetrics[i].height;
                    lm.baseline = lineMetrics[i].baseline;
                    lms->push_back(std::move(lm));
                }
                return true;
            }
        }

        ubassert(false);
        return false;
    }

    bool DWTextLayout::hitTestPoint(
        float x, float y, bool* is_trailing, bool* is_inside, HitTestInfo* info)
    {
        BOOL _is_trailing;
        BOOL _is_inside;
        DWRITE_HIT_TEST_METRICS metrics;
        HRESULT hr = text_layout_->HitTestPoint(x, y, &_is_trailing, &_is_inside, &metrics);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        *is_trailing = _is_trailing == TRUE;
        *is_inside = _is_inside == TRUE;
        info->pos = metrics.textPosition;
        info->length = metrics.length;
        info->rect.left = metrics.left;
        info->rect.top = metrics.top;
        info->rect.right = metrics.left + metrics.width;
        info->rect.bottom = metrics.top + metrics.height;

        return true;
    }

    bool DWTextLayout::hitTestTextRange(
        size_t pos, size_t len, float org_x, float org_y, std::vector<HitTestInfo>* info)
    {
        ubassert(pos <= std::numeric_limits<UINT32>::max());
        ubassert(len <= std::numeric_limits<UINT32>::max());

        UINT32 count;
        HRESULT hr = text_layout_->HitTestTextRange(
            UINT32(pos), UINT32(len), org_x, org_y, nullptr, 0, &count);
        if (hr == E_NOT_SUFFICIENT_BUFFER && count > 0) {
            std::unique_ptr<DWRITE_HIT_TEST_METRICS[]> metrics(new DWRITE_HIT_TEST_METRICS[count]);
            hr = text_layout_->HitTestTextRange(
                UINT32(pos), UINT32(len), org_x, org_y, metrics.get(), count, &count);
            if (SUCCEEDED(hr)) {
                for (uint32_t i = 0; i < count; ++i) {
                    HitTestInfo ht;
                    ht.pos = metrics[i].textPosition;
                    ht.length = metrics[i].length;
                    ht.rect.left = metrics[i].left;
                    ht.rect.top = metrics[i].top;
                    ht.rect.right = metrics[i].left + metrics[i].width;
                    ht.rect.bottom = metrics[i].top + metrics[i].height;
                    info->push_back(std::move(ht));
                }
                return true;
            }
        }

        ubassert(false);
        return false;
    }

    bool DWTextLayout::hitTestTextPos(
        size_t pos, bool is_trailing, PointF* pt, HitTestInfo* info)
    {
        ubassert(pos <= std::numeric_limits<UINT32>::max());

        FLOAT x, y;
        DWRITE_HIT_TEST_METRICS metrics;
        HRESULT hr = text_layout_->HitTestTextPosition(UINT32(pos), is_trailing ? TRUE : FALSE, &x, &y, &metrics);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        if (pt) {
            pt->x = x;
            pt->y = y;
        }

        info->pos = metrics.textPosition;
        info->length = metrics.length;
        info->rect.left = metrics.left;
        info->rect.top = metrics.top;
        info->rect.right = metrics.left + metrics.width;
        info->rect.bottom = metrics.top + metrics.height;

        return true;
    }

    void DWTextLayout::draw(Canvas* c, float x, float y) {
        text_layout_->Draw(c, text_renderer_.get(), x, y);
    }

    float DWTextLayout::getMaxLineHeight() {
        float max_height = 0;
        std::vector<LineMetrics> metrics;
        if (getLineMetrics(&metrics)) {
            for (const auto& m : metrics) {
                max_height = std::max(m.height, max_height);
            }
        }
        return max_height;
    }

}
}