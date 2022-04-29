// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TEXT_VIEW_H_
#define UKIVE_VIEWS_TEXT_VIEW_H_

#include "ukive/graphics/colors/color.h"
#include "ukive/text/editable.h"
#include "ukive/text/text_action_menu_callback.h"
#include "ukive/text/text_input_client.h"
#include "ukive/text/text_layout.h"
#include "ukive/views/view.h"


namespace ukive {

    class TextBlink;
    class InputMethodConnection;
    class TextActionMenu;
    class TextKeyListener;
    class DWTextDrawingEffect;

    class TextView : public View,
        public Editable::EditWatcher,
        public TextActionMenuCallback,
        public TextInputClient
    {
    public:
        explicit TextView(Context c);
        TextView(Context c, AttrsRef attrs);
        ~TextView();

        void onAttachedToWindow(Window* w) override;
        void onDetachFromWindow() override;

        void autoWrap(bool enable);
        void setIsEditable(bool editable);
        void setIsSelectable(bool selectable);
        void setSpace(const Padding& space);

        bool isAutoWrap() const;
        bool isEditable() const;
        bool isSelectable() const;

        void setText(const std::u16string_view& text);
        void setTextSize(int size);
        void setTextColor(const Color& color);
        void setTextAlignment(TextLayout::Alignment alignment);
        void setParagraphAlignment(TextLayout::Alignment alignment);
        void setTextStyle(TextLayout::FontStyle style);
        void setTextWeight(TextLayout::FontWeight weight);
        void setFontFamilyName(const std::u16string_view& font);
        void setLineSpacing(TextLayout::LineSpacing method, float spacing);

        const std::u16string& getText() const;
        Editable* getEditable() const;
        int getTextSize() const;

        void setSelection(size_t position);
        void setSelection(size_t start, size_t end);
        void drawSelection(size_t start, size_t end);
        std::u16string getSelection() const;
        size_t getSelectionStart() const;
        size_t getSelectionEnd() const;
        bool hasSelection() const;

        size_t getTextPositionAtPoint(int text_x, int text_y) const;
        bool isTextAtPoint(
            int text_x, int text_y,
            size_t* hit_pos = nullptr) const;
        bool isTextAtPoint(
            int text_x, int text_y,
            size_t position, size_t length, size_t* hit_pos = nullptr) const;
        RectF getSelectionBounds(size_t start, size_t end) const;

        void computeVisibleRegion(RectF* region);

    protected:
        // Editable::EditWatcher
        void onTextChanged(
            Editable* editable,
            size_t start, size_t old_end, size_t new_end, Editable::Reason r) override;
        void onSelectionChanged(
            size_t ns, size_t ne, size_t os, size_t oe, Editable::Reason r) override;
        void onSpanChanged(
            Span* span, SpanChange action, Editable::Reason r) override;

        // TextActionMenuCallback
        bool onCreateActionMode(TextActionMenu* mode, Menu* menu) override;
        bool onPrepareActionMode(TextActionMenu* mode, Menu* menu) override;
        bool onActionItemClicked(TextActionMenu* mode, MenuItem* item) override;
        void onDestroyActionMode(TextActionMenu* mode) override;
        void onGetContentPosition(int* x, int* y) override;

        // View
        void onDraw(Canvas* canvas) override;
        Size onDetermineSize(const SizeInfo& info) override;
        bool onInputEvent(InputEvent* e) override;

        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;
        void onBoundsChanged(const Rect& new_bounds, const Rect& old_bounds) override;
        void onFocusChanged(bool get_focus) override;
        void onEnableChanged(bool enabled) override;
        void onWindowFocusChanged(bool window_focus) override;
        void onScrollChanged(
            int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) override;
        bool onCheckIsTextEditor() override;
        InputMethodConnection* onCreateInputConnection() override;

        // TextInputClient
        void onTICBeginProcess() override;
        void onTICEndProcess() override;
        void onTICRedrawSelection() override;
        bool isTICEditable() const override;
        Editable* getTICEditable() const override;
        Window* getTICHostWindow() const override;
        void getTICBounds(Rect* bounds) const override;
        void getTICSelectionBounds(
            size_t sel_start, size_t sel_end, Rect* bounds, bool* clipped) const override;

    private:
        using super = View;

        void initTextView();

        int determineWidth(const SizeInfo::Value& width);
        int computeVerticalScrollRange();
        int computeHorizontalScrollRange();

        void computeTextOffsetAtViewTop();
        int computeVerticalScrollOffsetFromTextPos(size_t off);
        int computeHorizontalScrollOffsetFromTextPos(size_t off);

        int determineVerticalScroll(int dy);
        int determineHorizontalScroll(int dx);

        void scrollVertToFit(bool consider_sel);
        void scrollHoriToFit(bool consider_sel);

        bool sendEventToSpans(InputEvent* e);

        int getTextWidth() const;
        int getTextHeight() const;

        bool getLineInfo(
            size_t position,
            size_t* line, float* height, size_t* count = nullptr);
        bool getLineHeight(size_t line, float* height);

        void blinkNavigator(int key);

        void locateTextBlink(size_t position);
        void locateTextBlink(int text_x, int text_y);
        bool getTextBlinkLocation(size_t position, Rect* out) const;
        void makeNewTextLayout(float max_width, float max_height, bool auto_wrap);

        void applyFontAttrSpans();
        void applyOtherSpans();
        void refreshFontAttrs();

        bool canCut() const;
        bool canCopy() const;
        bool canPaste() const;
        bool canSelectAll() const;

        void performCut();
        void performCopy();
        void performPaste();
        void performSelectAll();

    private:
        // 为支持撤销而创建的结构体（未完成）。
        struct UndoBlock {
            std::u16string text;
            size_t start;
            size_t oldEnd;
            size_t newEnd;
        };

        struct SelectionBlock {
            size_t start;
            size_t length;
            RectF rect;
        };

        Editable* base_text_;
        std::unique_ptr<TextLayout> text_layout_;

        TextBlink* text_blink_;
        TextActionMenu* text_action_mode_;
        InputMethodConnection* input_connection_;
        TextKeyListener* text_key_listener_;

        uint64_t process_ref_;

        int font_size_;
        std::u16string font_family_name_;
        TextLayout::Alignment text_alignment_;
        TextLayout::Alignment paragraph_alignment_;
        TextLayout::FontWeight font_weight_;
        TextLayout::FontStyle font_style_;

        Color text_color_;
        Color sel_bg_color_;

        float line_spacing_;
        TextLayout::LineSpacing line_spacing_method_;

        bool is_auto_wrap_;
        bool is_editable_;
        bool is_selectable_;

        int prev_x_, prev_y_;
        bool is_plkey_down_;
        bool is_prkey_down_;
        bool is_plkey_down_on_text_;

        SizeInfo::Value parent_width_;

        size_t last_sel_;
        size_t first_sel_;
        float last_sel_left_;

        Padding space_;
        int vert_offset_ = 0;
        int hori_offset_ = 0;
        size_t text_pos_at_view_top_ = 0;
        std::vector<SelectionBlock> sel_list_;
    };

}

#endif  // UKIVE_VIEWS_TEXT_VIEW_H_