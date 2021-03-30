// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/page/page.h"

#include "ukive/views/view.h"


namespace ukive {

    Page::Page(Window* w)
        : window_(w) {}

    View* Page::create(LayoutView* parent) {
        if (!is_created_ && !content_view_) {
            content_view_ = onCreate(parent);
            if (content_view_) {
                is_created_ = true;
            }
        }
        return content_view_;
    }

    void Page::initialize() {
        if (is_created_) {
            onInitialize();
        }
    }

    void Page::show(bool show) {
        if (is_created_) {
            bool changed;
            if (show) {
                changed = content_view_->getVisibility() != View::SHOW;
            } else {
                changed = content_view_->getVisibility() == View::SHOW;
            }

            if (changed) {
                is_showing_ = show;
                content_view_->setVisibility(show ? View::SHOW : View::HIDE);
                onShow(show);
            }
        }
    }

    void Page::destroy() {
        if (is_created_) {
            onDestroy();

            content_view_ = nullptr;
            is_created_ = false;
        }
    }

    bool Page::isCreated() const {
        return is_created_;
    }

    bool Page::isShowing() const {
        return is_showing_;
    }

    Window* Page::getWindow() const {
        return window_;
    }

    View* Page::getContentView() const {
        return content_view_;
    }

}
