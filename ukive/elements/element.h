// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_ELEMENT_H_
#define UKIVE_ELEMENTS_ELEMENT_H_

#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Canvas;
    class Context;
    class Element;
    class Window;

    class ElementCallback {
    public:
        virtual ~ElementCallback() = default;

        virtual void onElementRequestDraw(Element* d) = 0;
    };

    class Element {
    public:
        enum Opacity {
            OPA_OPAQUE,
            OPA_SEMILUCENT,
            OPA_TRANSPARENT,
        };

        enum ElementState {
            STATE_NONE,
            STATE_PRESSED,
            STATE_FOCUSED,
            STATE_HOVERED,
            STATE_DISABLED,
        };

        Element();
        virtual ~Element() = default;

        void setBounds(const Rect& rect);
        void setBounds(int left, int top, int width, int height);
        void setCallback(ElementCallback* callback);

        bool setState(int state);
        void setHotspot(int x, int y);
        bool setParentFocus(bool focus);

        bool resetState();
        void notifyAttachedToWindow(Window* w);
        void notifyDetachedFromWindow();
        void notifyContextChanged(const Context& context);

        int getState() const;
        int getPrevState() const;
        Rect getBounds() const;
        ElementCallback* getCallback() const;
        Window* getWindow() const;
        bool isAttachedToWindow() const;

        virtual void draw(Canvas* canvas) = 0;

        virtual Opacity getOpacity() const;

        virtual int getContentWidth() const;
        virtual int getContentHeight() const;

    protected:
        void requestDraw();

        virtual bool onFocusChanged(bool focus);
        virtual void onBoundChanged(const Rect& new_bound);
        virtual bool onStateChanged(int new_state, int prev_state);
        virtual bool onStateReset();

        virtual void onAttachedToWindow(Window* w) {}
        virtual void onDetachedFromWindow() {}
        virtual void onContextChanged(const Context& context) {}

        int start_x_, start_y_;

    private:
        int state_;
        int prev_state_;
        bool is_parent_has_focus_;
        bool is_attached_to_window_ = false;
        Rect bounds_;
        Window* window_ = nullptr;
        ElementCallback* callback_;
    };

}

#endif  // UKIVE_ELEMENTS_ELEMENT_H_