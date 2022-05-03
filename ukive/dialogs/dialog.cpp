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
        levitator_ = std::make_shared<Levitator>();
        levitator_->setShadowRadius(c.dp2pxi(8.f));
        levitator_->setOutsideTouchable(false);
        levitator_->setDismissByTouchOutside(false);
        levitator_->setBackground(new ColorElement(Color::White));
        levitator_->setLayoutSize(View::LS_AUTO, View::LS_AUTO);
        levitator_->setEventListener(this);
        levitator_->setLayoutMargin(
            { c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8), c.dp2pxi(8) });
    }

    Dialog::~Dialog() {}

    void Dialog::show(Window* parent, int x, int y) {
        if (levitator_->isShowing()) {
            return;
        }

        auto v = onCreate(parent->getContext());
        levitator_->setContentView(v);

        Color bg = Color::Black;
        bg.a = 0.25f;

        auto anchor = parent->getRootLayout();
        anchor->setShadeBackground(new ColorElement(bg));

        levitator_->show(parent, x, y);
    }

    void Dialog::show(Window* parent, int gravity) {
        if (levitator_->isShowing()) {
            return;
        }

        auto v = onCreate(parent->getContext());
        levitator_->setContentView(v);

        Color bg = Color::Black;
        bg.a = 0.25f;

        Levitator::SnapInfo info;
        info.is_discretized = false;

        auto anchor = parent->getRootLayout();
        anchor->setShadeBackground(new ColorElement(bg));
        levitator_->show(anchor, gravity, info);
    }

    void Dialog::close() {
        if (!levitator_->isShowing()) {
            return;
        }

        onDestroy();

        levitator_->dismiss();

        if (is_own_by_myself_) {
            delete this;
        }
    }

    void Dialog::setOwnership(bool myself) {
        is_own_by_myself_ = myself;
    }

    void Dialog::onRequestDismissByTouchOutside(Levitator* lev) {
        close();
    }

}
