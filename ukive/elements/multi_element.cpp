// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "multi_element.h"

#include "utils/log.h"


namespace ukive {

    MultiElement::MultiElement()
        : Element() {}

    MultiElement::~MultiElement() {}

    void MultiElement::add(Element* element) {
        if (!element) {
            return;
        }

        ubassert(!element->isAttachedToWindow());
        list_.push_back(std::shared_ptr<Element>(element));

        if (isAttachedToWindow() && !element->isAttachedToWindow()) {
            ubassert(getWindow());
            element->notifyAttachedToWindow(getWindow());
        }
    }

    void MultiElement::remove(Element* element) {
        if (!element) {
            return;
        }

        for (auto it = list_.begin();
            it != list_.end();)
        {
            if ((*it).get() == element) {
                it = list_.erase(it);
            } else {
                ++it;
            }
        }

        if (isAttachedToWindow() && element->isAttachedToWindow()) {
            element->notifyDetachedFromWindow();
        }
    }

    void MultiElement::onBoundChanged(const Rect& new_bound) {
        for (auto& e : list_) {
            e->setBounds(new_bound);
        }
    }

    bool MultiElement::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = false;
        for (auto& e : list_) {
            need_redraw |= e->setState(new_state);
        }

        return need_redraw;
    }

    void MultiElement::onAttachedToWindow(Window* w) {
        for (auto& e : list_) {
            ubassert(!e->isAttachedToWindow());
            e->notifyAttachedToWindow(w);
        }
    }

    void MultiElement::onDetachedFromWindow() {
        for (auto& e : list_) {
            ubassert(e->isAttachedToWindow());
            e->notifyDetachedFromWindow();
        }
    }

    void MultiElement::onContextChanged(const Context& context) {
        for (auto& e : list_) {
            e->notifyContextChanged(context);
        }
    }

    void MultiElement::draw(Canvas* canvas) {
        for (auto& e : list_) {
            e->draw(canvas);
        }
    }

    Element::Opacity MultiElement::getOpacity() const {
        if (list_.empty()) {
            return OPA_TRANSPARENT;
        }
        return OPA_OPAQUE;
    }

}