// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_INPUT_EVENT_DELEGATE_H_
#define UKIVE_VIEWS_INPUT_EVENT_DELEGATE_H_


namespace ukive {

    class View;
    class LayoutView;
    class InputEvent;

    class OnInputEventDelegate {
    public:
        virtual ~OnInputEventDelegate() = default;

        /**
         * View 中的 onInputEvent 代理。
         * @return 返回 true 表示拦截当前事件，不再继续传递，并且使得 onInputEvent 返回 ret；
         *         返回 false 则继续传递到 onInputEvent。
         */
        virtual bool onInputReceived(View* v, InputEvent* e, bool* ret) = 0;
    };

    class OnHookInputEventDelegate {
    public:
        virtual ~OnHookInputEventDelegate() = default;

        /**
         * LayoutView 中的 onHookInputEvent 代理。
         * @return 返回 true 表示拦截当前事件，不再继续传递，并且
         *         使得 onHookInputEvent 返回 ret；
         *         返回 false 则继续传递到 onHookInputEvent。
         */
        virtual bool onHookInputReceived(LayoutView* v, InputEvent* e, bool* ret) = 0;
    };

}

#endif  // UKIVE_VIEWS_INPUT_EVENT_DELEGATE_H_