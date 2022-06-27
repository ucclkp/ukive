// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_VIEW_DELEGATE_H_
#define UKIVE_VIEWS_VIEW_DELEGATE_H_


namespace ukive {

    class View;
    class Canvas;
    class LayoutView;
    class InputEvent;

    class ViewDelegate {
    public:
        virtual ~ViewDelegate() = default;

        /**
         * View 中的 onInputEvent 代理。
         * @return 返回 true 表示拦截当前事件，不再继续传递，并且使得 onInputEvent 返回 ret；
         *         返回 false 则继续传递到 onInputEvent。
         */
        virtual bool onInputReceived(
            View* v, InputEvent* e, bool* ret) { return false; }

        /**
         * LayoutView 中的 onHookInputEvent 代理。
         * @return 返回 true 表示拦截当前事件，不再继续传递，并且
         *         使得 onHookInputEvent 返回 ret；
         *         返回 false 则继续传递到 onHookInputEvent。
         */
        virtual bool onHookInputReceived(
            LayoutView* v, InputEvent* e, bool* ret) { return false; }

        /**
         * View 中紧接 onDraw 之前调用。
         */
        virtual void onDrawReceived(View* v, Canvas* c) {}

        /**
         * View 中紧接 onDrawOverChildren 之后调用。
         */
        virtual void onDrawOverChildrenReceived(View* v, Canvas* c) {}
    };

}

#endif  // UKIVE_VIEWS_VIEW_DELEGATE_H_