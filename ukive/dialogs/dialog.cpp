// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "dialog.h"

#include "ukive/elements/color_element.h"
#include "ukive/views/layout/root_layout.h"
#include "ukive/window/window.h"


namespace ukive {

    Dialog::Dialog(Context c) {
        inner_window_ = std::make_shared<InnerWindow>();
        inner_window_->setShadowRadius(c.dp2pxi(8.f));
        inner_window_->setOutsideTouchable(false);
        inner_window_->setDismissByTouchOutside(false);
        inner_window_->setBackground(new ColorElement(Color::White));
        inner_window_->setWidth(View::LS_AUTO);
        inner_window_->setHeight(View::LS_AUTO);
        inner_window_->setEventListener(this);
    }

    Dialog::~Dialog() {}

    void Dialog::show(Window* parent, int x, int y) {
        if (inner_window_->isShowing()) {
            return;
        }

        auto v = onCreate(parent->getContext());
        inner_window_->setContentView(v);

        Color bg = Color::Black;
        bg.a = 0.25f;

        auto anchor = parent->getRootLayout();
        anchor->setShadeBackground(new ColorElement(bg));

        inner_window_->show(parent, x, y);
    }

    void Dialog::show(Window* parent, int gravity) {
        if (inner_window_->isShowing()) {
            return;
        }

        auto v = onCreate(parent->getContext());
        inner_window_->setContentView(v);

        Color bg = Color::Black;
        bg.a = 0.25f;

        auto anchor = parent->getRootLayout();
        anchor->setShadeBackground(new ColorElement(bg));
        inner_window_->show(anchor, gravity);
    }

    void Dialog::close() {
        if (!inner_window_->isShowing()) {
            return;
        }

        onDestroy();

        inner_window_->dismiss();

        if (is_own_by_myself_) {
            delete this;
        }
    }

    void Dialog::setOwnership(bool myself) {
        is_own_by_myself_ = myself;
    }

    void Dialog::onRequestDismissByTouchOutside(InnerWindow* iw) {
        close();
    }

}
