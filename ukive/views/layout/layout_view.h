// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_LAYOUT_VIEW_H_
#define UKIVE_VIEWS_LAYOUT_LAYOUT_VIEW_H_

#include <vector>

#include "utils/stl_utils.h"

#include "ukive/views/view.h"
#include "ukive/event/input_event.h"


namespace ukive {

    class OnHookInputEventDelegate;

    class LayoutView : public View {
    public:
        explicit LayoutView(Context c);
        LayoutView(Context c, AttrsRef attrs);
        ~LayoutView();

        void setHookInputEventDelegate(OnHookInputEventDelegate* d);

        bool dispatchInputEvent(InputEvent* e) override;
        void dispatchWindowFocusChanged(bool focus) override;
        void dispatchContextChanged(const Context& context) override;
        void dispatchAttachedToWindow(Window* w) override;
        void dispatchDetachFromWindow() override;

        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override {}

        virtual bool onHookInputEvent(InputEvent* e);
        virtual LayoutInfo* makeExtraLayoutInfo() const;
        virtual LayoutInfo* makeExtraLayoutInfo(AttrsRef attrs) const;
        virtual bool isValidExtraLayoutInfo(LayoutInfo* lp) const;

        bool isLayoutView() const override;

        /**
         * 向当前 LayoutView 的尾部添加子 View。
         * @param v 将被添加到当前 LayoutView 的子 View，v 不能有父母。
         * @param req_layout 指定是否请求重布局。
         */
        void addView(View* v, bool req_layout = true);

        /**
         * 向当前 LayoutView 的指定索引位置中添加子 View。
         * @param index v 将被添加到的位置。当前位置的 View 后移。
         * @param v 将被添加到当前 LayoutView 的子 View，v 不能有父母。
         * @param req_layout 指定是否请求重布局。
         */
        void addView(size_t index, View* v, bool req_layout = true);

        /**
         * 移除 LayoutView 中的指定子 View。
         * @param v 要移除的子 View。
         * @param del 指定是否要删除已被移除的 v。
         * @param req_layout 指定是否请求重布局。
         */
        void removeView(View* v, bool del = true, bool req_layout = true);

        /**
         * 移除 LayoutView 中的指定子 View。
         * @param index 要移除的子 View 的索引。
         * @param del 指定是否要删除已被移除的 v。
         * @param req_layout 指定是否请求重布局。
         */
        void removeView(size_t index, bool del = true, bool req_layout = true);

        /**
         * 移除 LayoutView 中所有的子 View。
         * @param del 指定是否要删除已被移除的子 View。
         * @param req_layout 指定是否请求重布局。
         */
        void removeAllViews(bool del = true, bool req_layout = true);

        size_t getChildCount() const;
        View* getChildById(int id) const;
        View* getChildAt(size_t index) const;

        View* findView(int id) override;

        void drawChild(Canvas* canvas, View* child);
        void drawChildren(Canvas* canvas);

        void determineChildSize(View* child, const SizeInfo& parent_info);
        void determineChildrenSize(const SizeInfo& parent_info);

        STL_VECTOR_ITERATORS(View*, views_);

    protected:
        // 重置 LayoutView 记录的事件拦截状态。
        // 从 onHookInputEvent() 返回 true 开始到 XXX_UP 消息结束，
        // 所有的事件会被拦截，对于触摸操作来说没有问题，但对鼠标操作来讲，鼠标即使
        // 不按下，也可以有 MOVE 和 WHEEL 操作，对于这些操作拦截之后会没有办法取消
        // 拦截状态，因此设置该方法来手动重置状态。
        void invalidateHookStatus();

        void dispatchDraw(Canvas* canvas) override;
        void dispatchAncestorVisibilityChanged(View* ancestor, int visibility) override;
        void dispatchAncestorEnableChanged(View* ancestor, bool enabled) override;
        void dispatchDiscardFocus() override;
        void dispatchDiscardPendingOperations() override;

        virtual bool dispatchPointerEvent(InputEvent* e);
        virtual bool dispatchKeyboardEvent(InputEvent* e);

        int getWrappedWidth();
        int getWrappedHeight();
        Size getWrappedSize(const SizeInfo& info);

    private:
        using super = View;

        void isolateChild(View* child, bool attached, bool del);

        void prepareHookingStatus(InputEvent* e);
        void updateHookingStatus(InputEvent* e);
        bool invokeOnHookInputEvent(InputEvent* e);

        std::vector<View*> views_;
        bool is_hooked_ = false;
        OnHookInputEventDelegate* iie_delegate_ = nullptr;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LAYOUT_VIEW_H_
