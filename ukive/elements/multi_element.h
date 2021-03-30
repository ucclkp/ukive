// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_MULTI_ELEMENT_H_
#define UKIVE_ELEMENTS_MULTI_ELEMENT_H_

#include <memory>
#include <vector>

#include "ukive/elements/element.h"


namespace ukive {

    class Canvas;

    class MultiElement : public Element {
    public:
        MultiElement();
        ~MultiElement();

        void add(Element* element);
        void remove(Element* element);

        void draw(Canvas* canvas) override;

        Opacity getOpacity() const override;

    protected:
        void onBoundChanged(const Rect& new_bound) override;
        bool onStateChanged(int new_state, int prev_state) override;
        void onAttachedToWindow(Window* w) override;
        void onDetachedFromWindow() override;
        void onContextChanged(const Context& context) override;

        std::vector<std::shared_ptr<Element>> list_;
    };

}

#endif  // UKIVE_ELEMENTS_MULTI_ELEMENT_H_