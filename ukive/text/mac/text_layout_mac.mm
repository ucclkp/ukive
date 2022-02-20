// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/mac/text_layout_mac.h"

#include <algorithm>

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/colors/color.h"

#import <Cocoa/Cocoa.h>
#import "text_inline_cell.h"


namespace ukive {
namespace mac {

    TextLayoutMac::TextLayoutMac() {}

    bool TextLayoutMac::make(
        const std::u16string_view &text,
        const std::u16string_view &font_name,
        float font_size,
        TextLayout::FontStyle style,
        TextLayout::FontWeight weight,
        const std::u16string_view &locale_name)
    {
        std::basic_string<unichar> u_text(text.begin(), text.end());
        auto ns_text = [[NSString alloc] initWithCharacters:
            u_text.c_str() length:u_text.length()];

        text_storage_ = [[NSTextStorage alloc] initWithString:ns_text];
        layout_mgr_ = [[NSLayoutManager alloc] init];
        text_container_ = [[NSTextContainer alloc] init];

        [layout_mgr_ addTextContainer:text_container_];
        [text_container_ release];

        [text_storage_ addLayoutManager:layout_mgr_];
        [layout_mgr_ release];

        std::basic_string<unichar> u_fn(font_name.begin(), font_name.end());
        auto ns_font_name = [[NSString alloc] initWithCharacters:
            u_fn.c_str() length:u_fn.length()];
        auto ns_font = [NSFont fontWithName:ns_font_name size:font_size];
        [ns_font_name release];

        if (!ns_font) {
            ns_font = [NSFont systemFontOfSize:font_size];
        }

        switch (style) {
            case FontStyle::ITALIC:
                ns_font = [[NSFontManager sharedFontManager] convertFont:ns_font
                                                          toHaveTrait:NSFontItalicTrait];
                break;
            default:
                break;
        }
        switch (weight) {
            case FontWeight::BOLD:
                ns_font = [[NSFontManager sharedFontManager] convertFont:ns_font
                                                          toHaveTrait:NSFontBoldTrait];
                break;
            case FontWeight::THIN:
                ns_font = [[NSFontManager sharedFontManager] convertFont:ns_font
                                                          toHaveTrait:NSFontCondensedTrait];
                break;
            default:
                break;
        }

        if (ns_font) {
            text_storage_.font = ns_font;
        }

        def_font_size_ = font_size;
        glyph_range_cached_ = false;
        [text_container_ setLineFragmentPadding:0.0];
        return true;
    }

    void TextLayoutMac::destroy() {
        [text_storage_ release];
    }

    void TextLayoutMac::setMaxWidth(float max_width) {
        auto max_height = text_container_.size.height;
        [text_container_ setSize:NSMakeSize(max_width, max_height)];
        glyph_range_cached_ = false;
    }

    void TextLayoutMac::setMaxHeight(float max_height) {
        auto max_width = text_container_.size.width;
        [text_container_ setSize:NSMakeSize(max_width, max_height)];
        glyph_range_cached_ = false;
    }

    void TextLayoutMac::setFontAttributes(const FontAttributes &attrs, const Range &range) {
        if (range.empty() ||
            range.length == 0 ||
            range.end() > text_storage_.length)
        {
            return;
        }

        auto font_size = def_font_size_;
        if (attrs.size.has_value()) {
            font_size = attrs.size.value();
        }

        NSFontTraitMask traits = 0;
        if (attrs.style.has_value()) {
            switch (attrs.style.value()) {
                case FontStyle::ITALIC:
                    traits |= NSFontItalicTrait;
                    break;
                default:
                    break;
            }
        }
        if (attrs.weight.has_value()) {
            switch (attrs.weight.value()) {
                case FontWeight::BOLD:
                    traits |= NSFontBoldTrait;
                    break;
                case FontWeight::THIN:
                    traits |= NSFontCondensedTrait;
                    break;
                default:
                    break;
            }
        }

        NSFont* font = nullptr;
        if (!attrs.name.empty()) {
            std::basic_string<unichar> u_an(attrs.name.begin(), attrs.name.end());
            auto ns_font_name = [[NSString alloc] initWithCharacters:
                u_an.c_str() length:u_an.length()];
            font = [NSFont fontWithName:ns_font_name size:font_size];
            [ns_font_name release];
        }
        if (!font) {
            font = [NSFont systemFontOfSize:font_size];
        }

        NSDictionary* attr_dic = @{NSFontAttributeName:font};
        [text_storage_ setAttributes:attr_dic range:NSMakeRange(range.pos, range.length)];
        [text_storage_ applyFontTraits:traits range:NSMakeRange(range.pos, range.length)];
    }

    void TextLayoutMac::setDrawingEffects(const DrawingEffects &effects, const Range &range) {
        if (range.empty() ||
            range.length == 0 ||
            range.end() > text_storage_.length)
        {
            return;
        }

        NSRange ns_range = NSMakeRange(range.pos, range.length);
        if (effects.has_underline) {
            [text_storage_ removeAttribute:NSUnderlineStyleAttributeName range:ns_range];

            NSNumber* underline_style = [NSNumber numberWithInt:NSUnderlineStyleSingle];
            [text_storage_ addAttribute:NSUnderlineStyleAttributeName value:underline_style range:ns_range];
        }
        if (effects.has_strikethrough) {
            [text_storage_ removeAttribute:NSStrikethroughStyleAttributeName range:ns_range];

            NSNumber* st_style = [NSNumber numberWithInt:NSUnderlineStyleSingle];
            [text_storage_ addAttribute:NSStrikethroughStyleAttributeName value:st_style range:ns_range];
        }
        if (effects.text_color.has_value()) {
            auto& color = *effects.text_color;
            NSColor* ns_color = [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
            [text_storage_ removeAttribute:NSForegroundColorAttributeName range:ns_range];
            [text_storage_ addAttribute:NSForegroundColorAttributeName value:ns_color range:ns_range];
        }
        if (effects.underline_color.has_value()) {
            auto& color = *effects.underline_color;
            NSColor* ns_color = [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
            [text_storage_ removeAttribute:NSUnderlineColorAttributeName range:ns_range];
            [text_storage_ addAttribute:NSUnderlineColorAttributeName value:ns_color range:ns_range];
        }
        if (effects.strikethrough_color.has_value()) {
            auto& color = *effects.strikethrough_color;
            NSColor* ns_color = [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
            [text_storage_ removeAttribute:NSStrikethroughColorAttributeName range:ns_range];
            [text_storage_ addAttribute:NSStrikethroughColorAttributeName value:ns_color range:ns_range];
        }
    }

    void TextLayoutMac::setInlineObject(TextInlineObject *tio, const Range &range) {
        if (range.empty() ||
            range.length == 0 ||
            range.end() > text_storage_.length)
        {
            return;
        }

        /**
         * macOS 下的嵌入式对象需要依赖一个占位符：NSAttachmentCharacter。
         * 只有在字符串中加入占位符，并且在占位符上设置 NSAttachmentAttributeName，NSLayoutManager
         * 才会认为这里有嵌入式对象。
         * 也就是说，如果要在某一个字符串范围上设置嵌入式对象，就得把这一个范围替换为占位符，这样会改变文本的内容。
         * 这与 Windows 上的 DirectWrite 的行为不同。
         */
        NSRange ns_range = NSMakeRange(range.pos, range.length);
        NSTextAttachment* attachment = [[NSTextAttachment alloc] initWithFileWrapper:nil];

        TextInlineCell* cell = [[TextInlineCell alloc] initTextCell:@""];
        cell.attachment = attachment;
        cell.enabled = YES;
        cell.delegate = tio;
        attachment.attachmentCell = cell;
        [cell release];

        NSAttributedString* attachment_str = [NSAttributedString attributedStringWithAttachment:attachment];
        [text_storage_ replaceCharactersInRange:ns_range withAttributedString:attachment_str];
    }

    void TextLayoutMac::setDefaultFontColor(const Color &color) {
        text_storage_.foregroundColor = [NSColor colorWithRed:color.r
                                                        green:color.g
                                                         blue:color.b
                                                        alpha:color.a];
    }

    void TextLayoutMac::setTextAlignment(Alignment align) {
        NSTextAlignment ns_align;
        switch (align) {
            case Alignment::LEADING:
                ns_align = NSTextAlignmentLeft;
                break;
            case Alignment::TRAILING:
                ns_align = NSTextAlignmentRight;
                break;
            case Alignment::CENTER:
            default:
                ns_align = NSTextAlignmentCenter;
                break;
        }

        [text_storage_ setAlignment:ns_align
                              range:NSMakeRange(0, text_storage_.length)];
    }

    void TextLayoutMac::setParagraphAlignment(Alignment align) {
        vert_align_ = align;
    }

    void TextLayoutMac::setTextWrapping(TextWrapping tw) {
        switch (tw) {
            case TextWrapping::NONE:
                [text_container_ setLineBreakMode:NSLineBreakByClipping];
                glyph_range_cached_ = false;
                break;
            case TextWrapping::WRAP:
                [text_container_ setLineBreakMode:NSLineBreakByWordWrapping];
                glyph_range_cached_ = false;
                break;
            default:
                break;
        }
    }

    void TextLayoutMac::setLineSpacing(LineSpacing ls, float spacing) {
        NSRange range = NSMakeRange(0, text_storage_.length);
        NSMutableParagraphStyle* style = [[NSMutableParagraphStyle alloc] init].autorelease;

        [text_storage_ removeAttribute:NSParagraphStyleAttributeName range:range];

        switch (ls) {
            case LineSpacing::DEFAULT:
                break;
            case LineSpacing::UNIFORM:
                [style setLineSpacing:spacing];
                [text_storage_ addAttribute:NSParagraphStyleAttributeName value:style range:range];
                break;
            case LineSpacing::PROPORTIONAL:
                [style setLineHeightMultiple:(std::max)(spacing - 1, 0.f)];
                [text_storage_ addAttribute:NSParagraphStyleAttributeName value:style range:range];
                break;
            default:
                break;
        }
    }

    float TextLayoutMac::getMaxWidth() const {
        return text_container_.size.width;
    }

    float TextLayoutMac::getMaxHeight() const {
        return text_container_.size.height;
    }

    bool TextLayoutMac::getTextMetrics(TextMetrics *tm) {
        calculateGlyphRange();
        auto rect = [layout_mgr_ usedRectForTextContainer:text_container_];
        tm->rect.set(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

        NSUInteger index = 0;
        NSUInteger line_count = 0;
        NSUInteger num_of_glyph = layout_mgr_.numberOfGlyphs;

        NSRange line_range;
        for (; index < num_of_glyph; ++line_count) {
            (void)[layout_mgr_ lineFragmentRectForGlyphAtIndex:index
                                                      effectiveRange:&line_range];
            index = NSMaxRange(line_range);
        }

        tm->line_count = uint32_t(line_count);
        return true;
    }

    bool TextLayoutMac::getLineMetrics(std::vector<LineMetrics> *lms) {
        NSUInteger index = 0;
        NSUInteger num_of_glyph = layout_mgr_.numberOfGlyphs;

        NSRange line_glyph_range;
        for (; index < num_of_glyph;) {
            auto rect = [layout_mgr_ lineFragmentRectForGlyphAtIndex:index
                                                      effectiveRange:&line_glyph_range];
            index = NSMaxRange(line_glyph_range);

            auto ch_range = [layout_mgr_ characterRangeForGlyphRange:line_glyph_range
                                                    actualGlyphRange:nil];

            LineMetrics metrics;
            metrics.length = uint32_t(ch_range.length);
            metrics.height = rect.size.height;
            // TODO: 获取基线位置
            metrics.baseline = metrics.height;

            lms->push_back(metrics);
        }

        return true;
    }

    bool TextLayoutMac::hitTestPoint(
        float x, float y, bool *is_trailing, bool *is_inside, HitTestInfo *info)
    {
        // 先看下指定点处的字形索引
        CGFloat fraction = 0;
        NSUInteger index = [layout_mgr_ glyphIndexForPoint:NSMakePoint(x, y)
                            inTextContainer:text_container_ fractionOfDistanceThroughGlyph:&fraction];

        // 找出字形范围
        NSRange glyph_range = NSMakeRange(index, 1);
        NSRange act_glyph_range;
        auto ch_range = [layout_mgr_ characterRangeForGlyphRange:glyph_range
                                                actualGlyphRange:&act_glyph_range];

        if (NSEqualRanges(act_glyph_range, glyph_range) == NO) {
            ch_range = [layout_mgr_ characterRangeForGlyphRange:act_glyph_range
                                               actualGlyphRange:nil];
        } else {
            act_glyph_range = glyph_range;
        }

        auto ns_rect = [layout_mgr_ boundingRectForGlyphRange:act_glyph_range
                                           inTextContainer:text_container_];
        RectF rect(
                  ns_rect.origin.x, ns_rect.origin.y,
                  ns_rect.size.width, ns_rect.size.height);

        *is_trailing = (fraction > 0.5f);
        *is_inside = rect.hit(x, y);
        info->pos = uint32_t(ch_range.location);
        info->length = uint32_t(ch_range.length);
        info->rect = rect;
        return true;
    }

    bool TextLayoutMac::hitTestTextRange(
        size_t pos, size_t len, float org_x, float org_y, std::vector<HitTestInfo> *info)
    {
        NSRange ch_range = NSMakeRange(pos, len);
        NSRange act_ch_range;
        auto glyph_range = [layout_mgr_ glyphRangeForCharacterRange:ch_range
                                               actualCharacterRange:&act_ch_range];

        if (NSEqualRanges(ch_range, act_ch_range) == NO) {
            glyph_range = [layout_mgr_ glyphRangeForCharacterRange:act_ch_range
                                              actualCharacterRange:nil];
        } else {
            act_ch_range = ch_range;
        }

        NSUInteger index = glyph_range.location;
        NSUInteger end_index = NSMaxRange(glyph_range);

        NSRange line_glyph_range;
        for (; index <= end_index;) {
            auto ns_rect = [layout_mgr_ lineFragmentUsedRectForGlyphAtIndex:index
                                                      effectiveRange:&line_glyph_range];
            RectF rect(
                ns_rect.origin.x, ns_rect.origin.y,
                ns_rect.size.width, ns_rect.size.height);

            NSRange fragment_range = NSIntersectionRange(glyph_range, line_glyph_range);
            if (NSEqualRanges(fragment_range, line_glyph_range) == NO) {
                auto fragment_rect = [layout_mgr_ boundingRectForGlyphRange:fragment_range
                                                            inTextContainer:text_container_];
                if (fragment_rect.origin.x > rect.left) {
                    rect.left = fragment_rect.origin.x;
                }
                if (fragment_rect.origin.x + fragment_rect.size.width < rect.right) {
                    rect.right = fragment_rect.origin.x + fragment_rect.size.width;
                }
            }

            auto line_ch_range = [layout_mgr_ characterRangeForGlyphRange:fragment_range
                                                         actualGlyphRange:nil];

            HitTestInfo hti;
            hti.pos = uint32_t(line_ch_range.location);
            hti.length = uint32_t(line_ch_range.length);
            hti.rect = rect;

            info->push_back(hti);

            index = NSMaxRange(line_glyph_range);
        }

        return !info->empty();
    }

    bool TextLayoutMac::hitTestTextPos(
        size_t pos, bool is_trailing, PointF *pt, HitTestInfo *info)
    {
        NSRange ch_range = NSMakeRange(pos, 1);
        NSRange act_ch_range;
        auto glyph_range = [layout_mgr_ glyphRangeForCharacterRange:ch_range
                                               actualCharacterRange:&act_ch_range];

        if (NSEqualRanges(ch_range, act_ch_range) == NO) {
            glyph_range = [layout_mgr_ glyphRangeForCharacterRange:act_ch_range
                                              actualCharacterRange:nil];
        } else {
            act_ch_range = ch_range;
        }

        auto ns_rect = [layout_mgr_ boundingRectForGlyphRange:glyph_range
                                              inTextContainer:text_container_];
        RectF rect(
                  ns_rect.origin.x, ns_rect.origin.y,
                  ns_rect.size.width, ns_rect.size.height);

        pt->x = is_trailing ? rect.right : rect.left;
        pt->y = rect.top;
        info->pos = uint32_t(act_ch_range.location);
        info->length = uint32_t(act_ch_range.length);
        info->rect = rect;
        return true;
    }

    void TextLayoutMac::draw(Canvas* c, float x, float y) {
        // 模拟文本在布局框中竖向居中的情况
        switch (vert_align_) {
            case Alignment::CENTER:
            {
                calculateGlyphRange();
                auto rect = [layout_mgr_ usedRectForTextContainer:text_container_];
                y += (text_container_.size.height - rect.size.height) / 2.f;
                break;
            }
            case Alignment::TRAILING:
            {
                calculateGlyphRange();
                auto rect = [layout_mgr_ usedRectForTextContainer:text_container_];
                y += text_container_.size.height - rect.size.height;
                break;
            }
            case Alignment::LEADING:
            default:
                break;
        };

        [text_storage_ enumerateAttribute:NSAttachmentAttributeName
                                  inRange:NSMakeRange(0, text_storage_.length)
                                  options:0
                               usingBlock:^(id  _Nullable value, NSRange range, BOOL * _Nonnull stop)
        {
            NSTextAttachment* attachment = (NSTextAttachment*)value;
            if (!attachment) return;
            TextInlineCell* cell = (TextInlineCell*)attachment.attachmentCell;
            if (!cell) return;
            cell.canvas = c;
        }];

        if (c->getOpacity() < 1.f) {
            auto context = [[NSGraphicsContext currentContext] CGContext];
            CGContextSaveGState(context);
            CGContextSetAlpha(context, c->getOpacity());

            calculateGlyphRange();
            auto glyph_range = NSMakeRange(glyph_range_.pos, glyph_range_.length);
            [layout_mgr_ drawGlyphsForGlyphRange:glyph_range atPoint:NSMakePoint(x, y)];

            CGContextRestoreGState(context);
        } else {
            calculateGlyphRange();
            auto glyph_range = NSMakeRange(glyph_range_.pos, glyph_range_.length);
            [layout_mgr_ drawGlyphsForGlyphRange:glyph_range atPoint:NSMakePoint(x, y)];
        }
    }

    void TextLayoutMac::nativeDraw(float x, float y) {
        // 模拟文本在布局框中竖向居中的情况
        switch (vert_align_) {
            case Alignment::CENTER:
            {
                calculateGlyphRange();
                auto rect = [layout_mgr_ usedRectForTextContainer:text_container_];
                y += (text_container_.size.height - rect.size.height) / 2.f;
                break;
            }
            case Alignment::TRAILING:
            {
                calculateGlyphRange();
                auto rect = [layout_mgr_ usedRectForTextContainer:text_container_];
                y += text_container_.size.height - rect.size.height;
                break;
            }
            case Alignment::LEADING:
            default:
                break;
        };

        calculateGlyphRange();
        auto glyph_range = NSMakeRange(glyph_range_.pos, glyph_range_.length);
        [layout_mgr_ drawGlyphsForGlyphRange:glyph_range atPoint:NSMakePoint(x, y)];
    }

    void TextLayoutMac::calculateGlyphRange() {
        if (glyph_range_cached_) {
            return;
        }

        auto range = [layout_mgr_ glyphRangeForTextContainer:text_container_];
        glyph_range_.set(range.location, range.length);

        glyph_range_cached_ = true;
    }

}
}
