// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "element.h"

#include "ukive/window/context.h"


namespace ukive {

    Element::Element()
        : start_x_(0),
          start_y_(0),
          state_(STATE_NONE),
          prev_state_(STATE_NONE),
          is_parent_has_focus_(false),
          callback_(nullptr) {}

    void Element::setBounds(const Rect &rect) {
        if (bounds_ == rect) {
            return;
        }

        bounds_ = rect;
        onBoundChanged(bounds_);
    }

    void Element::setBounds(int left, int top, int width, int height) {
        if (bounds_.left == left
            && bounds_.top == top
            && bounds_.right == left + width
            && bounds_.bottom == top + height)
        {
            return;
        }

        bounds_.set(left, top, width, height);

        onBoundChanged(bounds_);
    }

    void Element::setCallback(ElementCallback* callback) {
        callback_ = callback;
    }

    bool Element::setState(int state) {
        if (state == state_) {
            return false;
        }

        prev_state_ = state_;
        state_ = state;

        return onStateChanged(state, prev_state_);
    }

    void Element::setHotspot(int x, int y) {
        start_x_ = x;
        start_y_ = y;
    }

    bool Element::setParentFocus(bool focus) {
        if (focus == is_parent_has_focus_) {
            return false;
        }

        is_parent_has_focus_ = focus;
        return onFocusChanged(is_parent_has_focus_);
    }

    bool Element::resetState() {
        if (prev_state_ == STATE_NONE &&
            state_ == STATE_NONE)
        {
            return false;
        }
        prev_state_ = STATE_NONE;
        state_ = STATE_NONE;

        return onStateReset();
    }

    void Element::notifyAttachedToWindow(Window* w) {
        window_ = w;
        is_attached_to_window_ = true;
        onAttachedToWindow(w);
    }

    void Element::notifyDetachedFromWindow() {
        onDetachedFromWindow();
        window_ = nullptr;
        is_attached_to_window_ = false;
    }

    void Element::notifyContextChanged(const Context& context) {
        onContextChanged(context);
    }

    int Element::getState() const {
        return state_;
    }

    int Element::getPrevState() const {
        return prev_state_;
    }

    Rect Element::getBounds() const {
        return bounds_;
    }

    ElementCallback* Element::getCallback() const {
        return callback_;
    }

    Window* Element::getWindow() const {
        return window_;
    }

    bool Element::isAttachedToWindow() const {
        return is_attached_to_window_;
    }

    Element::Opacity Element::getOpacity() const {
        return OPA_OPAQUE;
    }

    void Element::requestDraw() {
        if (callback_) {
            callback_->onElementRequestDraw(this);
        }
    }

    bool Element::onFocusChanged(bool focus) {
        return false;
    }

    void Element::onBoundChanged(const Rect& new_bound) {}

    bool Element::onStateChanged(int new_state, int prev_state) {
        return false;
    }

    bool Element::onStateReset() {
        return false;
    }

    int Element::getContentWidth() const {
        return -1;
    }

    int Element::getContentHeight() const {
        return -1;
    }

}