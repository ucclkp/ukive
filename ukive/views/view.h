// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_VIEW_H_
#define UKIVE_VIEWS_VIEW_H_

#include <memory>
#include <string>

#include "ukive/animation/view_animator_params.h"
#include "ukive/elements/element.h"
#include "ukive/graphics/rect.hpp"
#include "ukive/graphics/padding.hpp"
#include "ukive/graphics/cursor.h"
#include "ukive/graphics/size.hpp"
#include "ukive/resources/attributes.h"
#include "ukive/views/size_info.h"
#include "ukive/window/context.h"


namespace ukive {

    class Canvas;
    class Element;
    class InputEvent;
    class LayoutInfo;
    class LayoutView;
    class OnClickListener;
    class OnInputEventDelegate;
    class ShadowEffect;
    class InputMethodConnection;
    class ViewAnimator;
    class Window;

    class View : public ElementCallback {
    public:
        enum Visibility {
            SHOW,
            HIDE,
            VANISHED
        };

        enum LayoutSizeMode {
            LS_AUTO = -1,
            LS_FILL = -2,
            LS_FREE = -3,
        };

        enum Outline {
            OUTLINE_RECT,
            OUTLINE_OVAL,
        };

        explicit View(Context c);
        View(Context c, AttrsRef attrs);
        virtual ~View();

        ViewAnimator* animate();
        ViewAnimatorParams* animeParams();

        void setId(int id);
        void setTag(int tag);
        void setCategory(int category);
        void setScrollX(int x);
        void setScrollY(int y);
        void setVisibility(int visibility);
        void setEnabled(bool enable);
        void setBackground(Element* element, bool owned = true);
        void setForeground(Element* element, bool owned = true);
        void setPadding(int lead, int top, int trail, int bottom);
        void setPressed(bool pressed);
        void setCurrentCursor(Cursor cursor);
        void setClickable(bool clickable);
        void setDoubleClickable(bool dbclkable);
        void setFocusable(bool focusable);
        void setTouchCapturable(bool capturable);
        void setShadowRadius(int radius);
        void setReceiveOutsideInputEvent(bool receive);
        void setMinimumWidth(int width);
        void setMinimumHeight(int height);
        void setOnClickListener(OnClickListener* l);
        void setOnInputEventDelegate(OnInputEventDelegate* d);
        void setOutline(Outline outline);

        void setLayoutSize(int width, int height);
        void setLayoutMargin(int start, int top, int end, int bottom);
        void setExtraLayoutInfo(LayoutInfo* info);

        // 一般由框架调用
        void setParent(LayoutView* parent);

        void offsetVertical(int dy);
        void offsetHorizontal(int dx);

        int getId() const;
        int getTag() const;
        int getCategory() const;
        int getScrollX() const;
        int getScrollY() const;
        int getLeft() const;
        int getTop() const;
        int getRight() const;
        int getBottom() const;
        int getWidth() const;
        int getHeight() const;
        int getShadowRadius() const;
        int getVisibility() const;
        int getOutline() const;
        OnClickListener* getClickListener() const;
        OnInputEventDelegate* getInputEventDelegate() const;
        const Size& getMinimumSize() const;
        const Size& getDeterminedSize() const;

        Padding& getPadding();
        const Padding& getPadding() const;
        LayoutView* getParent() const;

        Size& getLayoutSize();
        const Size& getLayoutSize() const;
        Margin& getLayoutMargin();
        const Margin& getLayoutMargin() const;

        LayoutInfo* getExtraLayoutInfo() const;

        Window* getWindow() const;
        Context getContext() const;
        Element* getBackground() const;
        Element* getForeground() const;

        // 相对于父 View 的 bounds
        Rect getBounds() const;

        // 相对于 RootLayout 的 bounds
        Rect getBoundsInRoot() const;

        // 相对于 window 的 bounds
        Rect getBoundsInWindow() const;

        // 相对于屏幕的 bounds
        Rect getBoundsInScreen() const;

        // 相对于自身的内容区坐标
        Rect getContentBounds() const;

        Rect getVisibleBounds() const;

        /**
         * 获取 View 超出边界的绘制区域。
         * 例如阴影。阴影在逻辑上属于 View，但不受 View 裁剪边界的限制。
         */
        Padding getBoundsExtension() const;

        std::weak_ptr<void> getCanary() const;

        /**
         * 使用 ViewAnimatorParams 生成的矩阵对 bounds 进行变换。
         */
        void transformBounds(Rect* bounds) const;

        bool isEnabled() const;
        bool isAttachedToWindow() const;
        bool isPressed() const;
        bool hasFocus() const;
        bool canGetFocus() const;
        bool isClickable() const;
        bool isDoubleClickable() const;
        bool isFocusable() const;
        bool isTouchCapturable() const;
        bool isLayouted() const;
        bool isLocalPointerInThis(InputEvent* e) const;
        bool isLocalPointerInThisVisible(InputEvent* e) const;
        bool isParentPointerInThis(InputEvent* e) const;
        bool isReceiveOutsideInputEvent() const;

        void scrollTo(int x, int y);
        void scrollBy(int dx, int dy);

        void measure(const SizeInfo& info);

        /**
         * 将该 View 放置于父 View 内的指定位置。
         * left, top, right 和 bottom 均相对于父 View。
         */
        void layout(const Rect& bounds);
        void draw(Canvas* canvas, const Rect& dirty_rect);

        virtual void requestDraw();
        virtual void requestDraw(const Rect& rect);
        virtual void requestDrawRelParent(const Rect& rect);
        virtual void requestLayout();

        void requestFocus();

        void discardFocus();
        void discardMouseCapture();
        void discardTouchCapture();
        void discardPendingOperations();

        void resetLayoutStatus();
        void resetLastInputView();

        virtual View* findView(int id);
        virtual bool dispatchInputEvent(InputEvent* e);
        virtual void dispatchWindowFocusChanged(bool focus);
        virtual void dispatchContextChanged(const Context& context);
        virtual void dispatchAncestorVisibilityChanged(View* ancestor, int visibility);
        virtual void dispatchAncestorEnableChanged(View* ancestor, bool enabled);
        virtual void dispatchAttachedToWindow(Window* w);
        virtual void dispatchDetachFromWindow();

        virtual bool isLayoutView() const;

    protected:
        // ElementCallback
        void onElementRequestDraw(Element* d) override;

        Size getPreferredSize(
            const SizeInfo& info, int pref_width, int pref_height);

        void performClick();
        void performDoubleClick();

        bool needDrawBackground();
        bool needDrawForeground();

        void drawBackground(Canvas* canvas);
        void drawForeground(Canvas* canvas);

        bool sendInputEvent(InputEvent* e, bool cancel = false);
        bool invokeOnInputEvent(InputEvent* e);
        void updateLastInputView(InputEvent* e, bool consumed, bool cancel);

        virtual void dispatchDraw(Canvas* canvas) {}
        virtual void dispatchDiscardFocus() {}
        virtual void dispatchDiscardPendingOperations() {}

        virtual void onAttachedToWindow(Window* w) {}
        virtual void onDetachFromWindow() {}

        virtual Size onDetermineSize(const SizeInfo& info);

        virtual void onBeforeLayout(
            Rect* new_bounds, const Rect& old_bounds) {}

        virtual void onBoundsChanging(
            const Rect& new_bounds, const Rect& old_bounds) {}
        virtual void onBoundsChanged(
            const Rect& new_bounds, const Rect& old_bounds) {}

        /**
         * 当该 View 被布局时调用。
         * @param new_bounds 相对于父 View 的新位置。
         * @param old_bounds 相对于父 View 的旧位置。
         */
        virtual void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) {}

        virtual void onAfterLayout(
            const Rect& new_bounds, const Rect& old_bounds) {}

        virtual bool onInputEvent(InputEvent* e);

        /**
         * 绘制 View 的内容。
         * 已考虑 Padding 的偏移。
         */
        virtual void onDraw(Canvas* canvas) {}
        virtual void onDrawOverChildren(Canvas* canvas) {}

        virtual bool onCheckIsTextEditor();
        virtual InputMethodConnection* onCreateInputConnection();
        virtual void onPreDraw() {}
        virtual void onPostDraw() {}

        virtual void onVisibilityChanged(int visibility) {}
        virtual void onAncestorVisibilityChanged(View* ancestor, int visibility);
        virtual void onEnableChanged(bool enabled) {}
        virtual void onAncestorEnableChanged(View* ancestor, bool enabled);
        virtual void onFocusChanged(bool get_focus);
        virtual void onWindowFocusChanged(bool window_focus);
        virtual void onContextChanged(const Context& context);
        virtual void onScrollChanged(
            int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) {}

        Rect dirty_rect_;
        std::shared_ptr<InputEvent> cur_ev_;

    private:
        void drawNormal(Canvas* c, bool has_bg, bool has_shadow);
        void drawWithReveal(Canvas* c, bool has_bg, bool has_shadow);
        void drawContent(Canvas* c);

        void updateBackgroundState();
        void updateForegroundState();
        void resetBackground();
        void resetForeground();

        void processPointerUp();
        bool processInputEvent(InputEvent* e);

        int id_;
        int tag_ = 0;
        int category_ = 0;

        // 相对于父 View 的位置
        Rect bounds_;
        Padding padding_;
        Outline outline_;

        Size layout_size_;
        Margin layout_margin_;

        int scroll_x_;
        int scroll_y_;

        Size determined_size_;
        SizeInfo saved_size_info_;
        Size min_size_;

        int visibility_;
        int shadow_radius_;

        bool has_focus_;
        bool has_focus_requesting_ = false;
        bool is_enabled_;
        bool is_attached_to_window_;
        bool is_pressed_;
        bool is_clickable_ = false;
        bool is_dbclkable_ = false;
        bool is_focusable_;
        bool is_touch_capturable_ = false;
        bool is_receive_outside_input_event_;
        bool is_mouse_down_;
        bool is_touch_down_;

        bool is_measured_ = false;
        bool is_layouted_ = false;
        bool need_layout_ = false;
        bool request_layout_ = false;

        Window* window_ = nullptr;
        Context context_;
        bool owned_bg_ = true;
        bool owned_fg_ = true;
        Element* bg_element_ = nullptr;
        Element* fg_element_ = nullptr;

        // 双击判定
        bool wait_for_dbclk_ = false;
        uint64_t first_clk_ts = 0;

        ViewAnimatorParams anime_params_;
        std::unique_ptr<ViewAnimator> animator_;
        std::unique_ptr<ShadowEffect> shadow_effect_;
        std::unique_ptr<LayoutInfo> layout_info_;

        LayoutView* parent_;
        OnClickListener* click_listener_ = nullptr;
        OnInputEventDelegate* ie_delegate_ = nullptr;
        InputMethodConnection* input_connection_;
    };

}

#endif  // UKIVE_VIEWS_VIEW_H_
