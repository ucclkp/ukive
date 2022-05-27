// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_ELEMENT_H_
#define UKIVE_ELEMENTS_ELEMENT_H_

#include <memory>
#include <vector>

#include "ukive/graphics/rect.hpp"
#include "ukive/window/context.h"


namespace ukive {

    class Canvas;
    class Element;

    class ElementCallback {
    public:
        virtual ~ElementCallback() = default;

        virtual void onElementRequestDraw(Element* d) = 0;
    };

    class Element {
    public:
        enum Shape {
            SHAPE_RECT,
            SHAPE_RRECT,
            SHAPE_OVAL
        };

        enum ElementState {
            STATE_NONE,
            STATE_PRESSED,
            STATE_FOCUSED,
            STATE_HOVERED,
            STATE_DISABLED,
        };

        Element();
        explicit Element(const Color& color);
        Element(Shape shape, const Color& color);
        virtual ~Element() = default;

        void setBounds(const Rect& rect);
        void setBounds(int x, int y, int width, int height);
        void setCallback(ElementCallback* callback);

        bool setState(int state);
        void setHotspot(int x, int y);
        bool setParentFocus(bool focus);

        bool resetState();
        void notifyContextChanged(Context::Type type, const Context& context);

        int getState() const;
        int getPrevState() const;
        Rect getBounds() const;
        ElementCallback* getCallback() const;
        bool isParentHasFocus() const;

        virtual void draw(Canvas* canvas);

        virtual bool isTransparent() const;

        virtual int getContentWidth() const;
        virtual int getContentHeight() const;

        void setShape(Shape shape);
        void setRadius(float radius);
        void setSolidEnable(bool enable);
        void setSolidColor(const Color& c);
        void setStrokeEnable(bool enable);
        void setStrokeWidth(float width);
        void setStrokeColor(const Color& color);

        void add(Element* element);
        void remove(Element* element);

        Shape getShape() const;
        const Color& getSolidColor() const;
        bool hasSolid() const;
        bool hasStroke() const;

    protected:
        void requestDraw();

        virtual bool onFocusChanged(bool focus);
        virtual void onBoundChanged(const Rect& new_bound);
        virtual bool onStateChanged(int new_state, int prev_state);
        virtual bool onStateReset();
        virtual void onContextChanged(
            Context::Type type, const Context& context);

        Point start_pos_{};

    private:
        Rect bounds_;
        int state_ = STATE_NONE;
        int prev_state_ = STATE_NONE;
        bool is_parent_has_focus_ = false;
        ElementCallback* callback_ = nullptr;

        Shape shape_;
        bool has_solid_;
        Color solid_color_;

        bool has_stroke_ = false;
        float stroke_width_ = 1.f;
        Color stroke_color_;

        float round_radius_ = 0;

        std::vector<std::shared_ptr<Element>> list_;
    };

}

#endif  // UKIVE_ELEMENTS_ELEMENT_H_