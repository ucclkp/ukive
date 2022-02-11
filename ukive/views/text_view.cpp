// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "text_view.h"

#include <algorithm>
#include <cmath>

#include "utils/strings/string_utils.hpp"

#include "ukive/text/span/inline_object_span.h"
#include "ukive/text/span/interactable_span.h"
#include "ukive/text/span/effect_span.h"
#include "ukive/text/span/text_attributes_span.h"
#include "ukive/text/input_method_connection.h"
#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/text/text_blink.h"
#include "ukive/system/clipboard_manager.h"
#include "ukive/text/range.hpp"
#include "ukive/text/text_action_menu.h"
#include "ukive/text/text_key_listener.h"
#include "ukive/window/window.h"
#include "ukive/text/span/span.h"
#include "ukive/graphics/canvas.h"
#include "ukive/menu/menu.h"
#include "ukive/menu/menu_item.h"
#include "ukive/resources/dimension_utils.h"

#include "necro/layout_constants.h"


namespace ukive {

    namespace {
        enum {
            MENU_ID_COPY = 1,
            MENU_ID_CUT,
            MENU_ID_PASTE,
            MENU_ID_SELECTALL,
        };

        enum {
            MENU_ORDER_COPY = 1,
            MENU_ORDER_CUT,
            MENU_ORDER_PASTE,
            MENU_ORDER_SELECTALL,
        };
    }

    TextView::TextView(Context c)
        : TextView(c, {}) {}

    TextView::TextView(Context c, AttrsRef attrs)
        : View(c, attrs),
          text_color_(Color::Black),
          sel_bg_color_(Color::Blue200),
          parent_width_(0, SizeInfo::FREEDOM)
    {
        is_selectable_ = resolveAttrBool(attrs, necro::kAttrTextViewIsSelectable, false);
        is_editable_ = resolveAttrBool(attrs, necro::kAttrTextViewIsEditable, false);
        if (is_editable_) {
            is_selectable_ = true;
        }

        is_auto_wrap_ = resolveAttrBool(attrs, necro::kAttrTextViewAutoWrap, true);

        auto text = resolveAttrString(attrs, necro::kAttrTextViewText, "");
        base_text_ = new Editable(utl::UTF8ToUTF16(text));
        base_text_->addEditWatcher(this);

        auto def_text_size = std::round(c.dp2px(15.f));
        font_size_ = int(resolveAttrDimension(
            getContext(), attrs, necro::kAttrTextViewTextSize, def_text_size));

        initTextView();
    }

    TextView::~TextView() {
        if (text_layout_) {
            text_layout_->destroy();
        }

        delete base_text_;
        delete text_blink_;
        delete text_key_listener_;
    }

    void TextView::initTextView() {
        is_plkey_down_ = false;
        is_prkey_down_ = false;
        is_plkey_down_on_text_ = false;

        line_spacing_ = 1.f;
        line_spacing_method_ = TextLayout::LineSpacing::DEFAULT;

        process_ref_ = 0;
        font_family_name_ = u"微软雅黑";
        text_action_mode_ = nullptr;
        text_alignment_ = TextLayout::Alignment::LEADING;
        paragraph_alignment_ = TextLayout::Alignment::LEADING;
        font_weight_ = TextLayout::FontWeight::NORMAL;
        font_style_ = TextLayout::FontStyle::NORMAL;

        text_blink_ = new TextBlink(this);
        text_blink_->setColor(Color::Blue500);

        input_connection_ = InputMethodConnection::create(this);
        text_key_listener_ = new TextKeyListener();

        makeNewTextLayout(0.f, 0.f, false);

        base_text_->setSelection(0);
        if (is_editable_) {
            locateTextBlink(0);
        }

        setFocusable(is_editable_ | is_selectable_);
    }

    int TextView::determineWidth(const SizeInfo::Value& width) {
        int final_width;

        int hori_padding = getPadding().hori() + space_.left + space_.right;

        int blink_thickness;
        if (is_editable_) {
            blink_thickness = int(std::ceil(text_blink_->getThickness()));
        } else {
            blink_thickness = 0;
        }

        switch (width.mode) {
        case SizeInfo::CONTENT: {
            text_layout_->setMaxWidth(float(std::max(width.val - hori_padding, 0)));

            final_width = std::min(getTextWidth() + blink_thickness + hori_padding, width.val);
            final_width = std::max(final_width, getMinimumSize().width);

            if (width.val != final_width) {
                text_layout_->setMaxWidth(float(std::max(final_width - hori_padding, 0)));
            }
            break;
        }

        case SizeInfo::FREEDOM:
            text_layout_->setMaxWidth(0.f);
            text_layout_->setTextWrapping(TextLayout::TextWrapping::NONE);

            final_width = getTextWidth() + blink_thickness + hori_padding;
            final_width = std::max(final_width, getMinimumSize().width);

            text_layout_->setMaxWidth(float(std::max(final_width - hori_padding, 0)));

            text_layout_->setTextWrapping(
                is_auto_wrap_ ? TextLayout::TextWrapping::WRAP : TextLayout::TextWrapping::NONE);
            break;

        case SizeInfo::DEFINED:
        default:
            text_layout_->setMaxWidth(float(std::max(width.val - hori_padding, 0)));
            final_width = width.val;
            break;
        }

        return final_width;
    }

    int TextView::computeVerticalScrollRange() {
        int text_height = getTextHeight();
        int content_height = getHeight()
            - getPadding().vert()
            - space_.top - space_.bottom;

        return std::max(0, text_height - content_height);
    }

    int TextView::computeHorizontalScrollRange() {
        int content_width = getWidth()
            - getPadding().hori() - space_.left - space_.right;
        return std::max(0, getTextWidth() - content_width);
    }

    void TextView::computeTextOffsetAtViewTop() {
        bool is_inside;
        bool is_trailing_hit;
        TextLayout::HitTestInfo metrics;

        if (text_layout_->hitTestPoint(
            float(getScrollX()), float(getScrollY()), &is_trailing_hit, &is_inside, &metrics))
        {
            text_pos_at_view_top_ = metrics.pos + (is_trailing_hit ? metrics.length : 0);
            hori_offset_ = getScrollX() - int(metrics.rect.left);
            vert_offset_ = getScrollY() - int(metrics.rect.top);
        }
    }

    int TextView::computeVerticalScrollOffsetFromTextPos(size_t off) {
        std::vector<TextLayout::HitTestInfo> hit_metrics;
        if (text_layout_->hitTestTextRange(off, 0, 0.f, 0.f, &hit_metrics)) {
            return int(hit_metrics[0].rect.top) - getScrollY();
        }

        return 0 - getScrollY();
    }

    int TextView::computeHorizontalScrollOffsetFromTextPos(size_t off) {
        std::vector<TextLayout::HitTestInfo> hit_metrics;
        if (text_layout_->hitTestTextRange(off, 0, 0.f, 0.f, &hit_metrics)) {
            return int(hit_metrics[0].rect.left) - getScrollX();
        }

        return 0 - getScrollX();
    }

    int TextView::determineVerticalScroll(int dy) {
        int final_y = getScrollY();
        int range = computeVerticalScrollRange();
        if (range > 0) {
            if (dy < 0) {
                final_y = std::max(0, getScrollY() + dy);
            } else if (dy > 0) {
                final_y = std::min(range, getScrollY() + dy);
            }
        }

        return final_y - getScrollY();
    }

    int TextView::determineHorizontalScroll(int dx) {
        int final_x = getScrollX();
        int range = computeHorizontalScrollRange();
        if (range > 0) {
            if (dx < 0) {
                final_x = std::max(0, getScrollX() + dx);
            } else if (dx > 0) {
                final_x = std::min(range, getScrollX() + dx);
            }
        }

        return final_x - getScrollX();
    }

    void TextView::scrollVertToFit(bool consider_sel) {
        bool scrolled = false;

        if (computeVerticalScrollRange() == 0) {
            if (getScrollY() != 0) {
                scrollTo(0, 0);
                scrolled = true;
            }
        } else {
            int scroll_offset = 0;

            if (consider_sel) {
                size_t selection;
                if (hasSelection()) {
                    selection = last_sel_;
                } else {
                    selection = getSelectionStart();
                }

                std::vector<TextLayout::HitTestInfo> metrics;
                if (text_layout_->hitTestTextRange(selection, 0, 0.f, 0.f, &metrics)) {
                    int line_top = int(metrics[0].rect.top);
                    int line_bottom = int(metrics[0].rect.bottom);
                    int content_height = getHeight()
                        - getPadding().vert() - space_.top - space_.bottom;

                    bool top_beyond = line_top <= getScrollY();
                    bool bottom_beyond = line_bottom > getScrollY() + content_height;
                    if (top_beyond && bottom_beyond) {
                        scroll_offset = line_top - getScrollY();
                    } else if (top_beyond) {
                        scroll_offset = line_top - getScrollY();
                    } else if (bottom_beyond) {
                        scroll_offset = line_bottom - (getScrollY() + content_height);
                    }
                }
            } else {
                //置于大小变化前的位置。
                auto prev_text_offset = text_pos_at_view_top_;

                bool is_inside;
                bool is_trailing_hit;
                TextLayout::HitTestInfo metrics;

                if (text_layout_->hitTestPoint(
                    float(getScrollX()), float(getScrollY()), &is_trailing_hit, &is_inside, &metrics))
                {
                    auto cur_text_offset = metrics.pos + (is_trailing_hit ? metrics.length : 0);
                    if (cur_text_offset != prev_text_offset) {
                        scroll_offset = computeVerticalScrollOffsetFromTextPos(prev_text_offset);
                    }
                }
            }

            //检查是否超出屏幕。
            int max_scroll_offset = computeVerticalScrollRange() - getScrollY();
            int offset_y = std::min(max_scroll_offset, scroll_offset);
            if (offset_y != 0) {
                scrollBy(0, offset_y);
                scrolled = true;
            }
        }
    }

    void TextView::scrollHoriToFit(bool consider_sel) {
        bool scrolled = false;

        if (computeHorizontalScrollRange() == 0) {
            if (getScrollX() != 0) {
                scrollTo(0, 0);
                scrolled = true;
            }
        } else {
            int scroll_offset = 0;

            if (consider_sel) {
                auto selection = hasSelection() ? last_sel_ : getSelectionStart();

                Rect rect;
                if (getTextBlinkLocation(selection, &rect)) {
                    int content_width = getWidth()
                        - getPadding().hori() - space_.left - space_.right;

                    bool left_beyond = rect.left <= getScrollX();
                    bool right_beyond = rect.right > getScrollX() + content_width;
                    if (left_beyond && right_beyond) {
                        scroll_offset = rect.left - getScrollX();
                    } else if (left_beyond) {
                        scroll_offset = rect.left - getScrollX();
                    } else if (right_beyond) {
                        scroll_offset = rect.right - (getScrollX() + content_width);
                    }
                }
            } else {
                //置于大小变化前的位置。
                auto prev_text_offset = text_pos_at_view_top_;

                bool is_inside;
                bool is_trailing_hit;
                TextLayout::HitTestInfo metrics;

                if (text_layout_->hitTestPoint(
                    float(getScrollX()), float(getScrollY()), &is_trailing_hit, &is_inside, &metrics))
                {
                    auto cur_text_offset = metrics.pos + (is_trailing_hit ? metrics.length : 0);
                    if (cur_text_offset != prev_text_offset) {
                        scroll_offset = computeHorizontalScrollOffsetFromTextPos(prev_text_offset);
                    }
                }
            }

            //检查是否超出屏幕。
            int min_scroll_offset = 0 - getScrollX();
            int max_scroll_offset = computeHorizontalScrollRange() - getScrollX();
            int offset_x = std::min(max_scroll_offset, scroll_offset);
            offset_x = std::max(min_scroll_offset, offset_x);
            if (offset_x != 0) {
                scrollBy(offset_x, 0);
                scrolled = true;
            }
        }
    }

    bool TextView::sendEventToSpans(InputEvent* e) {
        if (!base_text_->isInteractable()) {
            return false;
        }

        if (e->getEvent() == InputEvent::EV_LEAVE_VIEW) {
            for (size_t i = 0; i < base_text_->getSpanCount(); ++i) {
                auto span = base_text_->getSpan(i);
                if (span->getType() == Span::INTERACTABLE) {
                    static_cast<InteractableSpan*>(span)->onInputEvent(e);
                }
            }
            return false;
        }

        if (e->isKeyboardEvent()) {
            return false;
        }

        auto x = e->getX() - getPadding().start - space_.left + getScrollX();
        auto y = e->getY() - getPadding().top - space_.top + getScrollY();

        size_t hit_pos;
        if (isTextAtPoint(x, y, &hit_pos)) {
            for (size_t i = base_text_->getSpanCount(); i > 0; --i) {
                auto span = base_text_->getSpan(i - 1);
                if (span->getType() == Span::INTERACTABLE &&
                    hit_pos >= span->getStart() && hit_pos < span->getEnd())
                {
                    return static_cast<InteractableSpan*>(span)->onInputEvent(e);
                }
            }
        }
        return false;
    }

    int TextView::getTextWidth() const {
        TextLayout::TextMetrics metrics;
        if (text_layout_->getTextMetrics(&metrics)) {
            return int(std::ceil(metrics.rect.width()));
        }
        return 0;
    }

    int TextView::getTextHeight() const {
        TextLayout::TextMetrics metrics;
        if (text_layout_->getTextMetrics(&metrics)) {
            return int(std::ceil(metrics.rect.height()));
        }
        return 0;
    }

    bool TextView::getLineInfo(
        size_t position, size_t* line, float* height, size_t* count)
    {
        std::vector<TextLayout::LineMetrics> line_metrics;
        if (text_layout_->getLineMetrics(&line_metrics)) {
            size_t line_text_pos_start = 0;
            for (size_t i = 0; i < line_metrics.size(); ++i) {
                size_t line_text_pos_end = line_text_pos_start + line_metrics[i].length;
                if (position >= line_text_pos_start && position <= line_text_pos_end) {
                    *line = i;
                    *height = line_metrics[i].height;
                    if (count) { *count = line_metrics.size(); }
                    return true;
                }

                line_text_pos_start = line_text_pos_end;
            }
        }

        return false;
    }

    bool TextView::getLineHeight(size_t line, float* height) {
        std::vector<TextLayout::LineMetrics> line_metrics;
        if (text_layout_->getLineMetrics(&line_metrics)) {
            if (line >= 0 && line < line_metrics.size()) {
                *height = line_metrics[line].height;
                return true;
            }
        }

        return false;
    }

    void TextView::blinkNavigator(int key) {
        if (key == Keyboard::KEY_LEFT) {
            auto start = base_text_->getSelectionStart();

            if (base_text_->hasSelection()) {
                base_text_->setSelection(start, Editable::Reason::USER_INPUT);
            } else {
                auto off = base_text_->getPrevOffset(start);
                if (off > 0) {
                    base_text_->setSelection(start - off, Editable::Reason::USER_INPUT);
                }
            }
        } else if (key == Keyboard::KEY_RIGHT) {
            auto end = base_text_->getSelectionEnd();

            if (base_text_->hasSelection()) {
                base_text_->setSelection(end, Editable::Reason::USER_INPUT);
            } else {
                auto off = base_text_->getNextOffset(end);
                if (off > 0) {
                    base_text_->setSelection(end + off, Editable::Reason::USER_INPUT);
                }
            }
        } else if (key == Keyboard::KEY_UP) {
            size_t start;
            if (hasSelection() &&
                (last_sel_ == base_text_->getSelectionStart() ||
                last_sel_ == base_text_->getSelectionEnd()))
            {
                start = last_sel_;
            } else {
                start = base_text_->getSelectionStart();
            }

            std::vector<TextLayout::HitTestInfo> range_metrics;
            if (text_layout_->hitTestTextRange(start, 0, 0.f, 0.f, &range_metrics)) {
                bool is_trailing, is_inside;
                TextLayout::HitTestInfo point_metrics;
                if (text_layout_->hitTestPoint(
                    range_metrics[0].rect.left, range_metrics[0].rect.top - 1.f,
                    &is_trailing, &is_inside, &point_metrics))
                {
                    base_text_->setSelection(
                        point_metrics.pos + (is_trailing ? point_metrics.length : 0),
                        Editable::Reason::USER_INPUT);
                }
            }
        } else if (key == Keyboard::KEY_DOWN) {
            size_t start;
            if (hasSelection() &&
                (last_sel_ == base_text_->getSelectionStart() ||
                last_sel_ == base_text_->getSelectionEnd()))
            {
                start = last_sel_;
            } else {
                start = base_text_->getSelectionStart();
            }

            std::vector<TextLayout::HitTestInfo> range_metrics;
            if (text_layout_->hitTestTextRange(start, 0, 0.f, 0.f, &range_metrics)) {
                bool is_trailing, is_inside;
                TextLayout::HitTestInfo point_metrics;
                if (text_layout_->hitTestPoint(
                    range_metrics[0].rect.left, range_metrics[0].rect.bottom,
                    &is_trailing, &is_inside, &point_metrics))
                {
                    base_text_->setSelection(
                        point_metrics.pos + (is_trailing ? point_metrics.length : 0),
                        Editable::Reason::USER_INPUT);
                }
            }
        }
    }

    void TextView::makeNewTextLayout(float max_width, float max_height, bool auto_wrap) {
        if (text_layout_) {
            text_layout_->destroy();
            text_layout_.reset();
        }

        text_layout_.reset(TextLayout::create());
        text_layout_->make(
            base_text_->getString(),
            font_family_name_,
            float(font_size_),
            font_style_,
            font_weight_,
            u"zh-CN");

        text_layout_->setMaxWidth(max_width);
        text_layout_->setMaxHeight(max_height);
        text_layout_->setLineSpacing(line_spacing_method_, line_spacing_);
        text_layout_->setTextWrapping(
            auto_wrap ? TextLayout::TextWrapping::WRAP : TextLayout::TextWrapping::NONE);
        text_layout_->setTextAlignment(text_alignment_);
        text_layout_->setParagraphAlignment(paragraph_alignment_);
        text_layout_->setDefaultFontColor(text_color_);

        applyFontAttrSpans();
        applyOtherSpans();
    }

    void TextView::applyFontAttrSpans() {
        for (size_t i = 0; i < base_text_->getSpanCount(); ++i) {
            auto span = base_text_->getSpan(i);

            Range range;
            range.pos = span->getStart();
            range.length = span->getEnd() - span->getStart();

            switch (span->getType()) {
            case Span::FONT_ATTRIBUTES:
            {
                auto attrs_span = static_cast<TextAttributesSpan*>(span);

                TextLayout::FontAttributes attrs;
                attrs.size = attrs_span->font_size;
                attrs.style = attrs_span->font_style;
                attrs.weight = attrs_span->font_weight;
                attrs.name = attrs_span->font_name;
                text_layout_->setFontAttributes(attrs, range);
                break;
            }
            default:
                break;
            }
        }
    }

    void TextView::applyOtherSpans() {
        for (size_t i = 0; i < base_text_->getSpanCount(); ++i) {
            auto span = base_text_->getSpan(i);

            Range range;
            range.pos = span->getStart();
            range.length = span->getEnd() - span->getStart();

            switch (span->getType()) {
            case Span::DRAWING_EFFECTS:
            {
                auto effects_span = static_cast<DrawingEffectsSpan*>(span);

                TextLayout::DrawingEffects effects;
                effects.has_underline = effects_span->has_underline;
                effects.has_strikethrough = effects_span->has_strikethrough;
                effects.has_custom_draw = effects_span->has_custom_draw;
                effects.text_color = effects_span->text_color;
                effects.underline_color = effects_span->underline_color;
                effects.strikethrough_color = effects_span->strikethrough_color;
                effects.tcd = effects_span->tcd;
                text_layout_->setDrawingEffects(effects, range);
                break;
            }

            case Span::INTERACTABLE:
                break;

            case Span::INLINE_OBJECT:
            {
                auto io_span = static_cast<InlineObjectSpan*>(span);
                text_layout_->setInlineObject(io_span, range);
                break;
            }

            default:
                break;
            }
        }
    }

    void TextView::refreshFontAttrs() {
        TextLayout::FontAttributes attrs;
        attrs.size = font_size_;
        attrs.style = font_style_;
        attrs.weight = font_weight_;
        attrs.name = font_family_name_;
        text_layout_->setFontAttributes(
            attrs, Range(0, base_text_->getString().length()));
        applyFontAttrSpans();
    }

    void TextView::locateTextBlink(size_t position) {
        Rect rect;
        if (getTextBlinkLocation(position, &rect)) {
            text_blink_->locate(rect);
        }
    }

    void TextView::locateTextBlink(int text_x, int text_y) {
        auto hitPosition = getTextPositionAtPoint(text_x, text_y);
        locateTextBlink(hitPosition);
    }

    bool TextView::getTextBlinkLocation(size_t position, Rect* out) const {
        /**
         * 这里 position 可取字符串长度，光标将定位在最后一个字符结尾。
         * 如果使用字符串长度减一，is_trailing 取 true 的话，如果最后一个字符是换行，
         * 则光标无法正确定位。
         *
         * TODO: 值得一提的是 macOS 系统上 hitTestTextPos() 当前
         * 获取的光标高度是和行高一致的，而 Windows 上光标高度则与内容一致，需要统一。
         */
        PointF pt;
        TextLayout::HitTestInfo metrics;
        if (text_layout_->hitTestTextPos(position, false, &pt, &metrics)) {
            text_blink_->calculateRect(
                pt.x, pt.y, pt.y + metrics.rect.height(), out);
            return true;
        }
        return false;
    }

    void TextView::onAttachedToWindow(Window* w) {
        View::onAttachedToWindow(w);
    }

    void TextView::onDetachFromWindow() {
        View::onDetachFromWindow();
    }

    void TextView::onFocusChanged(bool get_focus) {
        super::onFocusChanged(get_focus);

        if (get_focus) {
            if (is_editable_) {
                text_blink_->show();
            }
        } else {
            if (text_action_mode_ != nullptr) {
                text_action_mode_->close();
            }

            text_blink_->hide();
            base_text_->setSelection(base_text_->getSelectionStart(), Editable::Reason::USER_INPUT);
            requestDraw();
        }
    }

    void TextView::onEnableChanged(bool enabled) {
        super::onEnableChanged(enabled);

        if (enabled) {
            text_layout_->setDefaultFontColor(text_color_);
        } else {
            Color text_color = text_color_;
            text_color.a *= 0.5f;
            text_layout_->setDefaultFontColor(text_color);
        }
    }

    void TextView::onWindowFocusChanged(bool window_focus) {
        super::onWindowFocusChanged(window_focus);

        if (hasFocus()) {
            if (window_focus) {
                if (is_editable_ && !hasSelection()) {
                    text_blink_->show();
                }
            } else {
                text_blink_->hide();
                if (text_action_mode_ != nullptr) {
                    text_action_mode_->close();
                }
            }
        }
    }

    void TextView::onScrollChanged(int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) {
        View::onScrollChanged(scroll_x, scroll_y, old_scroll_x, old_scroll_y);

        computeTextOffsetAtViewTop();
    }

    void TextView::onDraw(Canvas* canvas) {
        canvas->save();
        canvas->translate(float(space_.left), float(space_.top));

        if (is_selectable_) {
            for (const auto& sel : sel_list_) {
                canvas->fillRect(sel.rect, sel_bg_color_);
            }
        }

        text_blink_->draw(canvas);
        text_layout_->draw(canvas, 0, 0);

        canvas->restore();
    }

    Size TextView::onDetermineSize(const SizeInfo& info) {
        parent_width_ = info.width;

        int final_width = determineWidth(info.width);
        int final_height;

        int vert_padding = getPadding().vert() + space_.top + space_.bottom;

        switch (info.height.mode) {
        case SizeInfo::CONTENT:
        {
            final_height = std::min(getTextHeight() + vert_padding, info.height.val);
            final_height = std::max(final_height, getMinimumSize().height);
            text_layout_->setMaxHeight(float(std::max(final_height - vert_padding, 0)));
            break;
        }

        case SizeInfo::FREEDOM:
            text_layout_->setMaxHeight(0);

            final_height = getTextHeight() + vert_padding;
            final_height = std::max(final_height, getMinimumSize().height);

            text_layout_->setMaxHeight(float(std::max(final_height - vert_padding, 0)));
            break;

        case SizeInfo::DEFINED:
        default:
            text_layout_->setMaxHeight(float(std::max(info.height.val - vert_padding, 0)));
            final_height = info.height.val;
            break;
        }

        return Size(final_width, final_height);
    }

    void TextView::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        if (text_action_mode_ && new_bounds != old_bounds) {
            text_action_mode_->invalidatePosition();
        }
    }

    void TextView::onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) {
        if (new_bounds.size() == old_bounds.size()) {
            return;
        }

        size_t sel_start = base_text_->getSelectionStart();
        size_t sel_end = base_text_->getSelectionEnd();

        if (sel_start == sel_end && is_editable_) {
            locateTextBlink(sel_start);
        } else if (sel_start != sel_end && is_selectable_) {
            drawSelection(sel_start, sel_end);
        }

        if (hasFocus() && is_editable_) {
            input_connection_->notifyTextLayoutChanged(InputMethodConnection::TLC_CHANGE);
        }

        scrollVertToFit(false);
        scrollHoriToFit(false);
    }

    bool TextView::onInputEvent(InputEvent* e) {
        bool result = View::onInputEvent(e);
        bool is_interactivable = is_selectable_ || is_editable_;

        result |= sendEventToSpans(e);

        switch (e->getEvent()) {
        case InputEvent::EV_LEAVE_VIEW:
        {
            is_plkey_down_ = false;
            is_plkey_down_on_text_ = false;
            is_prkey_down_ = false;
            if (e->isMouseEvent()) {
                setCurrentCursor(Cursor::ARROW);
            }
            result = true;
            break;
        }

        case InputEvent::EVM_SCROLL_ENTER:
        {
            if (is_selectable_) {
                setCurrentCursor(Cursor::IBEAM);
            }
            result = true;
            break;
        }

        case InputEvent::EVM_DOWN:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT ||
                e->getMouseKey() == InputEvent::MK_RIGHT)
            {
                if (text_action_mode_ != nullptr) {
                    text_action_mode_->close();
                }

                bool is_down = true;
                bool is_down_on_text = isTextAtPoint(
                    e->getX() - getPadding().start - space_.left + getScrollX(),
                    e->getY() - getPadding().top - space_.top + getScrollY());

                if (is_selectable_ && (is_down_on_text || is_editable_)) {
                    setCurrentCursor(Cursor::IBEAM);
                }

                if (e->getMouseKey() == InputEvent::MK_LEFT) {
                    is_plkey_down_ = is_down;
                    is_plkey_down_on_text_ = is_down_on_text;
                } else if (e->getMouseKey() == InputEvent::MK_RIGHT) {
                    is_prkey_down_ = is_down;
                    //mIsMouseRightKeyDownOnText = isMouseKeyDownOnText;
                }

                if (e->getMouseKey() != InputEvent::MK_RIGHT || !hasSelection()) {
                    first_sel_ = getTextPositionAtPoint(
                        e->getX() - getPadding().start - space_.left + getScrollX(),
                        e->getY() - getPadding().top - space_.top + getScrollY());

                    if (input_connection_->isCompositing()) {
                        // 输入法在候选阶段时，用鼠标修改当前选择位置并不会使
                        // 输入法改变输入位置，所以干脆就直接结束掉。
                        input_connection_->terminateComposition();
                    }
                    base_text_->setSelection(first_sel_, Editable::Reason::USER_INPUT);
                }
            }
            result |= is_interactivable;
            break;
        }

        case InputEvent::EVT_DOWN:
        {
            result |= is_interactivable;
            break;
        }

        case InputEvent::EVM_UP:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                is_plkey_down_ = false;
                is_plkey_down_on_text_ = false;

                if (is_selectable_
                    && (isTextAtPoint(
                        e->getX() - getPadding().start - space_.left + getScrollX(),
                        e->getY() - getPadding().top - space_.top + getScrollY()) || is_editable_))
                {
                    setCurrentCursor(Cursor::IBEAM);
                } else {
                    setCurrentCursor(Cursor::ARROW);
                }
            } else if (e->getMouseKey() == InputEvent::MK_RIGHT) {
                prev_x_ = e->getX();
                prev_y_ = e->getY();
                is_prkey_down_ = false;

                if (is_editable_ || is_selectable_) {
                    text_action_mode_ = getWindow()->startTextActionMenu(this);
                }
            }
            result |= is_interactivable;
            break;
        }

        case InputEvent::EVT_UP:
        {
            if (text_action_mode_ != nullptr) {
                text_action_mode_->close();
            }

            bool is_down_on_text = isTextAtPoint(
                e->getX() - getPadding().start - space_.left + getScrollX(),
                e->getY() - getPadding().top - space_.top + getScrollY());

            first_sel_ = getTextPositionAtPoint(
                e->getX() - getPadding().start - space_.left + getScrollX(),
                e->getY() - getPadding().top - space_.top + getScrollY());
            base_text_->setSelection(first_sel_, Editable::Reason::USER_INPUT);

            result |= is_interactivable;
            break;
        }

        case InputEvent::EVM_MOVE:
        {
            if (is_plkey_down_) {
                size_t start = first_sel_;
                size_t end = getTextPositionAtPoint(
                    e->getX() - getPadding().start - space_.left + getScrollX(),
                    e->getY() - getPadding().top - space_.top + getScrollY());

                last_sel_ = end;

                if (start > end) {
                    auto tmp = start;
                    start = end;
                    end = tmp;
                }

                base_text_->setSelection(start, end, Editable::Reason::USER_INPUT);
            } else {
                if (is_selectable_
                    && (isTextAtPoint(
                        e->getX() - getPadding().start - space_.left + getScrollX(),
                        e->getY() - getPadding().top - space_.top + getScrollY()) || is_editable_))
                {
                    setCurrentCursor(Cursor::IBEAM);
                } else {
                    setCurrentCursor(Cursor::ARROW);
                }
            }
            result |= is_interactivable;
            break;
        }

        case InputEvent::EVT_MOVE:
        {
            result |= is_interactivable;
            break;
        }

        case InputEvent::EVM_WHEEL:
        {
            int direction;
            if (e->getWheelValue() > 0) {
                direction = 1;
            } else if (e->getWheelValue() < 0) {
                direction = -1;
            } else {
                direction = 0;
            }

            int dy;
            if (line_spacing_method_ == TextLayout::LineSpacing::DEFAULT) {
                dy = -font_size_ * direction;
            } else if (line_spacing_method_ == TextLayout::LineSpacing::UNIFORM) {
                dy = -int(font_size_ * line_spacing_ * direction);
            } else {
                dy = -font_size_ * direction;
            }

            dy = determineVerticalScroll(dy * 3);
            if (dy != 0) {
                scrollBy(0, dy);
                result |= is_interactivable;
            }
            break;
        }

        case InputEvent::EVK_CHARS:
        {
            text_key_listener_->onChars(
                base_text_, is_editable_, is_selectable_, e->getKeyboardChars());
            break;
        }

        case InputEvent::EVK_DOWN:
        {
            text_key_listener_->onKeyDown(
                base_text_, is_editable_, is_selectable_, e->getKeyboardKey());
            if (is_editable_ && is_selectable_) {
                blinkNavigator(e->getKeyboardKey());
            }
            break;
        }

        case InputEvent::EVK_UP:
        {
            text_key_listener_->onKeyUp(
                base_text_, is_editable_, is_selectable_, e->getKeyboardKey());
            break;
        }

        default:
            break;
        }

        return result;
    }

    bool TextView::onCheckIsTextEditor() {
        return is_editable_;
    }

    InputMethodConnection* TextView::onCreateInputConnection() {
        return input_connection_;
    }

    void TextView::onTICBeginProcess() {
        ++process_ref_;
    }

    void TextView::onTICEndProcess() {
        --process_ref_;
    }

    void TextView::onTICRedrawSelection() {
        auto start = getSelectionStart();
        auto end = getSelectionEnd();

        if (start == end) {
            if (is_editable_) {
                locateTextBlink(start);
                requestDraw();
            }
        } else {
            drawSelection(start, end);
        }
    }

    bool TextView::isTICEditable() const {
        return isEditable();
    }

    Editable* TextView::getTICEditable() const {
        return base_text_;
    }

    Window* TextView::getTICHostWindow() const {
        return getWindow();
    }

    void TextView::getTICBounds(Rect* bounds) const {
        *bounds = getBoundsInScreen();
    }

    void TextView::getTICSelectionBounds(
        size_t sel_start, size_t sel_end, Rect* bounds, bool* clipped) const
    {
        Rect _bounds = getBoundsInScreen();
        Rect text_bounds = Rect(getSelectionBounds(sel_start, sel_end));

        text_bounds.offset(
            _bounds.left + getPadding().start + space_.left - getScrollX(),
            _bounds.top + getPadding().top + space_.top - getScrollY());

        *bounds = text_bounds;
    }

    void TextView::autoWrap(bool enable) {
        if (is_auto_wrap_ == enable) {
            return;
        }

        is_auto_wrap_ = enable;
        text_layout_->setTextWrapping(
            enable ? TextLayout::TextWrapping::WRAP : TextLayout::TextWrapping::NONE);

        requestLayout();
        requestDraw();
    }

    void TextView::setIsEditable(bool editable) {
        if (editable == is_editable_) {
            return;
        }

        is_editable_ = editable;

        if (editable) {
            setFocusable(true);
            if (hasFocus()) {
                text_blink_->show();
            }
        } else {
            setFocusable(is_selectable_);
            text_blink_->hide();
        }

        requestLayout();
        requestDraw();
    }

    void TextView::setIsSelectable(bool selectable) {
        if (selectable == is_selectable_) {
            return;
        }

        is_selectable_ = selectable;

        if (selectable) {
            setFocusable(true);
        } else {
            setFocusable(is_editable_);

            if (!sel_list_.empty()) {
                sel_list_.clear();
                requestDraw();
            }
        }
    }

    void TextView::setSpace(const Rect& space) {
        if (space != space_) {
            space_ = space;
            requestLayout();
            requestDraw();
        }
    }

    bool TextView::isAutoWrap() const {
        return is_auto_wrap_;
    }

    bool TextView::isEditable() const {
        return is_editable_;
    }

    bool TextView::isSelectable() const {
        return is_selectable_;
    }

    void TextView::setText(const std::u16string& text) {
        base_text_->replace(text, 0, base_text_->length());
        base_text_->setSelection(0);
    }

    void TextView::setTextSize(int size) {
        if (size == font_size_) {
            return;
        }

        font_size_ = size;
        refreshFontAttrs();

        requestLayout();
        requestDraw();
    }

    void TextView::setTextColor(const Color& color) {
        text_color_ = color;
        if (isEnabled()) {
            text_layout_->setDefaultFontColor(color);
        } else {
            auto disable_color = color;
            disable_color.a *= 0.5f;
            text_layout_->setDefaultFontColor(disable_color);
        }

        requestDraw();
    }

    void TextView::setTextAlignment(TextLayout::Alignment alignment) {
        if (text_alignment_ == alignment) {
            return;
        }

        text_alignment_ = alignment;
        text_layout_->setTextAlignment(alignment);
    }

    void TextView::setParagraphAlignment(TextLayout::Alignment alignment) {
        if (paragraph_alignment_ == alignment) {
            return;
        }

        paragraph_alignment_ = alignment;
        text_layout_->setParagraphAlignment(alignment);
    }

    void TextView::setTextStyle(TextLayout::FontStyle style) {
        if (font_style_ == style) {
            return;
        }

        font_style_ = style;
        refreshFontAttrs();

        requestLayout();
        requestDraw();
    }

    void TextView::setTextWeight(TextLayout::FontWeight weight) {
        if (font_weight_ == weight) {
            return;
        }

        font_weight_ = weight;
        refreshFontAttrs();

        requestLayout();
        requestDraw();
    }

    void TextView::setFontFamilyName(const std::u16string& font) {
        if (font_family_name_ == font) {
            return;
        }

        font_family_name_ = font;
        refreshFontAttrs();

        requestLayout();
        requestDraw();
    }

    void TextView::setLineSpacing(TextLayout::LineSpacing method, float spacing) {
        line_spacing_ = spacing;
        line_spacing_method_ = method;
        text_layout_->setLineSpacing(line_spacing_method_, line_spacing_);

        requestLayout();
        requestDraw();
    }

    const std::u16string& TextView::getText() const {
        return base_text_->getString();
    }

    Editable* TextView::getEditable() const {
        return base_text_;
    }

    int TextView::getTextSize() const {
        return font_size_;
    }

    void TextView::setSelection(size_t position) {
        base_text_->setSelection(position);
    }

    void TextView::setSelection(size_t start, size_t end) {
        base_text_->setSelection(start, end);
    }

    void TextView::drawSelection(size_t start, size_t end) {
        if (end <= start) {
            return;
        }

        std::vector<TextLayout::HitTestInfo> hit_metrics;
        if (!text_layout_->hitTestTextRange(
            start, end - start, 0.f, 0.f, &hit_metrics))
        {
            return;
        }

        sel_list_.clear();

        for (const auto& ht : hit_metrics) {
            SelectionBlock block;

            int extraWidth = 0;
            auto t_pos = ht.pos;
            auto t_length = ht.length;

            //一行中只有\n或\r\n时，添加一个一定宽度的Selection。
            if ((t_length == 1 && base_text_->at(t_pos) == u'\n') ||
                (t_length == 2 && base_text_->at(t_pos) == u'\r' && base_text_->at(t_pos + 1) == u'\n'))
            {
                extraWidth = font_size_;
            }
            //一行中句尾有\n或\r\n时，句尾添加一个一定宽度的Selection。
            else if ((t_length > 0 &&
                base_text_->at(t_pos + t_length - 1) == u'\n') ||
                (t_length > 1 && base_text_->at(t_pos + t_length - 2) == u'\r' &&
                    base_text_->at(t_pos + t_length - 1) == u'\n'))
            {
                extraWidth = font_size_;
            }

            block.rect.left = std::floor(ht.rect.left);
            block.rect.top = std::floor(ht.rect.top);
            block.rect.right = std::ceil(ht.rect.right + extraWidth);
            block.rect.bottom = std::ceil(ht.rect.bottom);
            block.start = t_pos;
            block.length = t_length;
            sel_list_.push_back(std::move(block));
        }

        requestDraw();
    }

    std::u16string TextView::getSelection() const {
        return base_text_->getSelection();
    }

    size_t TextView::getSelectionStart() const {
        return base_text_->getSelectionStart();
    }

    size_t TextView::getSelectionEnd() const {
        return base_text_->getSelectionEnd();
    }

    bool TextView::hasSelection() const {
        return base_text_->hasSelection();
    }

    size_t TextView::getTextPositionAtPoint(int text_x, int text_y) const {
        bool is_inside;
        bool is_trailing_hit;
        TextLayout::HitTestInfo metrics;

        if (!text_layout_->hitTestPoint(
            float(text_x), float(text_y), &is_trailing_hit, &is_inside, &metrics))
        {
            return 0;
        }

        return metrics.pos + (is_trailing_hit ? metrics.length : 0);
    }

    bool TextView::isTextAtPoint(int text_x, int text_y, size_t* hit_pos) const {
        bool is_inside;
        bool is_trailing_hit;
        TextLayout::HitTestInfo metrics;

        if (!text_layout_->hitTestPoint(
            float(text_x), float(text_y), &is_trailing_hit, &is_inside, &metrics))
        {
            return false;
        }

        if (hit_pos) {
            *hit_pos = metrics.pos + (is_trailing_hit ? metrics.length : 0);
        }

        return is_inside;
    }

    bool TextView::isTextAtPoint(
        int text_x, int text_y,
        size_t position, size_t length, size_t* hit_pos) const
    {
        bool is_inside;
        bool is_trailing_hit;
        TextLayout::HitTestInfo metrics;

        if (!text_layout_->hitTestPoint(
            float(text_x), float(text_y), &is_trailing_hit, &is_inside, &metrics))
        {
            return false;
        }

        if (hit_pos) {
            *hit_pos = metrics.pos + (is_trailing_hit ? metrics.length : 0);
        }

        if (is_inside) {
            if (metrics.pos >= position && metrics.pos <= position + length - 1) {
                return true;
            }
        }

        return false;
    }

    RectF TextView::getSelectionBounds(size_t start, size_t end) const {
        if (end < start) {
            return RectF();
        }

        RectF bounds;

        if (end == start) {
            PointF p;
            TextLayout::HitTestInfo metrics;
            if (!text_layout_->hitTestTextPos(start, false, &p, &metrics)) {
                return {};
            }

            bounds.left = p.x;
            bounds.top = p.y;
            bounds.right = p.x;
            bounds.bottom = p.y + metrics.rect.height();
        } else {
            std::vector<TextLayout::HitTestInfo> hit_metrics;
            if (!text_layout_->hitTestTextRange(start, end - start, 0.f, 0.f, &hit_metrics)) {
                return {};
            }

            for (size_t i = 0; i < hit_metrics.size(); i++) {
                if (i == 0) {
                    bounds = hit_metrics[i].rect;
                    continue;
                }
                bounds.join(hit_metrics[i].rect);
            }
        }

        return bounds;
    }

    void TextView::computeVisibleRegion(RectF* region) {
        region->left = 0.f + getScrollX();
        region->top = 0.f + getScrollY();
        region->right = region->left + text_layout_->getMaxWidth();
        region->bottom = region->top + text_layout_->getMaxHeight();
    }

    void TextView::onTextChanged(
        Editable* editable,
        size_t start, size_t old_end, size_t new_end, Editable::Reason r)
    {
        auto max_width = text_layout_->getMaxWidth();
        auto max_height = text_layout_->getMaxHeight();
        makeNewTextLayout(max_width, max_height, isAutoWrap());

        // 开启自动换行时，如果 View 是随内容扩大的话，
        // 使用输入法在文本末尾输入文字时可能出现候选框跳变的现象，
        // 如果候选框是根据当前输入文字（Composition）的位置来决定其显示位置的话。
        // 因为刚刚输入的文字会被换到下一行，并在下次布局后回到正确的位置。
        // 为了能在第一时间获知文本布局框的正确大小，应提前进行测量工作。
        if (is_auto_wrap_ &&
            getLayoutSize().width == LS_AUTO)
        {
            determineWidth(parent_width_);
        }

        requestLayout();
        requestDraw();

        if (process_ref_ == 0 &&
            hasFocus() &&
            is_editable_)
        {
            InputMethodConnection::TextChangeInfo info;
            info.start = start;
            info.old_end = old_end;
            info.new_end = new_end;
            input_connection_->notifyTextChanged(false, info);
        }

        scrollVertToFit(false);
        scrollHoriToFit(false);
    }

    void TextView::onSelectionChanged(
        size_t ns, size_t ne,
        size_t os, size_t oe, Editable::Reason r)
    {
        bool locate_blink = false;
        if (ns == ne) {
            if (os != oe) {
                if (text_action_mode_) {
                    text_action_mode_->close();
                }

                sel_list_.clear();
                requestDraw();
            }

            if (is_editable_) {
                locate_blink = true;
            }
        } else {
            setSelection(ns, ne);
            text_blink_->hide();
            drawSelection(ns, ne);
        }

        if (process_ref_ == 0 && hasFocus() && is_editable_) {
            input_connection_->notifyTextSelectionChanged();
        }

        scrollVertToFit(true);
        scrollHoriToFit(true);

        if (locate_blink) {
            locateTextBlink(ns);
            if (hasFocus()) {
                text_blink_->show();
            }
        }
    }

    void TextView::onSpanChanged(Span* span, SpanChange action, Editable::Reason r) {
        Range range;
        range.pos = span->getStart();
        range.length = span->getEnd() - span->getStart();

        switch (span->getType()) {
        case Span::FONT_ATTRIBUTES:
        {
            if (action == REMOVE) {
                TextLayout::FontAttributes attrs;
                attrs.is_removed = true;
                attrs.size = font_size_;
                attrs.style = font_style_;
                attrs.weight = font_weight_;
                attrs.name = font_family_name_;
                text_layout_->setFontAttributes(attrs, range);
            } else {
                auto attrs_span = static_cast<TextAttributesSpan*>(span);

                TextLayout::FontAttributes attrs;
                attrs.is_removed = false;
                attrs.size = attrs_span->font_size;
                attrs.style = attrs_span->font_style;
                attrs.weight = attrs_span->font_weight;
                attrs.name = attrs_span->font_name;
                text_layout_->setFontAttributes(attrs, range);
            }
            break;
        }

        case Span::DRAWING_EFFECTS:
        {
            if (action == REMOVE) {
                TextLayout::DrawingEffects effects;
                effects.is_removed = true;
                effects.has_underline = false;
                effects.has_strikethrough = false;
                effects.has_custom_draw = false;
                effects.tcd = nullptr;
                text_layout_->setDrawingEffects(effects, range);
            } else {
                auto effects_span = static_cast<DrawingEffectsSpan*>(span);

                TextLayout::DrawingEffects effects;
                effects.is_removed = false;
                effects.has_underline = effects_span->has_underline;
                effects.has_strikethrough = effects_span->has_strikethrough;
                effects.has_custom_draw = effects_span->has_custom_draw;
                effects.text_color = effects_span->text_color;
                effects.underline_color = effects_span->underline_color;
                effects.strikethrough_color = effects_span->strikethrough_color;
                effects.tcd = effects_span->tcd;
                text_layout_->setDrawingEffects(effects, range);
            }
            break;
        }

        case Span::INTERACTABLE:
            break;

        case Span::INLINE_OBJECT:
        {
            if (action == REMOVE) {
                text_layout_->setInlineObject(nullptr, range);
            } else {
                auto io_span = static_cast<InlineObjectSpan*>(span);
                text_layout_->setInlineObject(io_span, range);
            }
            break;
        }

        default:
            break;
        }
    }

    bool TextView::canCut() const {
        return is_editable_ && base_text_->hasSelection();
    }

    bool TextView::canCopy() const {
        return base_text_->hasSelection();
    }

    bool TextView::canPaste() const {
        if (is_editable_) {
            auto content = ClipboardManager::getFromClipboard();
            return !content.empty();
        }
        return false;
    }

    bool TextView::canSelectAll() const {
        if (base_text_->length() == 0) {
            return false;
        }

        if (base_text_->getSelectionStart() == 0 &&
            base_text_->getSelectionEnd() == base_text_->length())
        {
            return false;
        }

        return true;
    }

    void TextView::performCut() {
        ClipboardManager::saveToClipboard(getSelection());
        base_text_->replace(u"", Editable::Reason::USER_INPUT);

        if (text_action_mode_) {
            text_action_mode_->close();
        }
    }

    void TextView::performCopy() {
        ClipboardManager::saveToClipboard(getSelection());

        if (text_action_mode_) {
            text_action_mode_->close();
        }
    }

    void TextView::performPaste() {
        auto content = ClipboardManager::getFromClipboard();
        if (hasSelection()) {
            base_text_->replace(content, Editable::Reason::USER_INPUT);
        } else {
            base_text_->insert(content, Editable::Reason::USER_INPUT);
        }

        if (text_action_mode_) {
            text_action_mode_->close();
        }
    }

    void TextView::performSelectAll() {
        setSelection(0, base_text_->length());
        text_action_mode_->invalidateMenu();
    }

    bool TextView::onCreateActionMode(TextActionMenu* mode, Menu* menu) {
        if (this->canCopy())
            menu->addItem(MENU_ID_COPY, MENU_ORDER_COPY, u"复制");

        if (this->canCut())
            menu->addItem(MENU_ID_CUT, MENU_ORDER_CUT, u"剪切");

        if (this->canPaste())
            menu->addItem(MENU_ID_PASTE, MENU_ORDER_PASTE, u"粘贴");

        if (this->canSelectAll())
            menu->addItem(MENU_ID_SELECTALL, MENU_ORDER_SELECTALL, u"全选");

        return true;
    }

    bool TextView::onPrepareActionMode(TextActionMenu* mode, Menu* menu) {
        bool can_copy = this->canCopy();
        MenuItem* copy_item = menu->findItem(MENU_ID_COPY);
        if (copy_item) {
            copy_item->setItemVisible(can_copy);
        } else if (can_copy) {
            menu->addItem(MENU_ID_COPY, MENU_ORDER_COPY, u"复制");
        }

        bool can_cut = this->canCut();
        MenuItem* cut_item = menu->findItem(MENU_ID_CUT);
        if (cut_item) {
            cut_item->setItemVisible(can_cut);
        } else if (can_cut) {
            menu->addItem(MENU_ID_CUT, MENU_ORDER_CUT, u"剪切");
        }

        bool can_paste = this->canPaste();
        MenuItem* paste_item = menu->findItem(MENU_ID_PASTE);
        if (paste_item) {
            paste_item->setItemVisible(can_paste);
        } else if (can_paste) {
            menu->addItem(MENU_ID_PASTE, MENU_ORDER_PASTE, u"粘贴");
        }

        bool can_select_all = this->canSelectAll();
        MenuItem* select_all_item = menu->findItem(MENU_ID_SELECTALL);
        if (select_all_item) {
            select_all_item->setItemVisible(can_select_all);
        } else if (can_select_all) {
            menu->addItem(MENU_ID_SELECTALL, MENU_ORDER_SELECTALL, u"全选");
        }

        return true;
    }

    bool TextView::onActionItemClicked(TextActionMenu* mode, MenuItem* item) {
        switch (item->getItemId()) {
        case MENU_ID_COPY:
            performCopy();
            break;
        case MENU_ID_CUT:
            performCut();
            break;
        case MENU_ID_PASTE:
            performPaste();
            break;
        case MENU_ID_SELECTALL:
            performSelectAll();
            break;
        default:
            break;
        }

        return true;
    }

    void TextView::onDestroyActionMode(TextActionMenu* mode) {
        text_action_mode_ = nullptr;
    }

    void TextView::onGetContentPosition(int* x, int* y) {
        auto bounds = getBoundsInRoot();

        *x = bounds.left + prev_x_ + 1;
        *y = bounds.top + prev_y_ + 1;
    }

}
