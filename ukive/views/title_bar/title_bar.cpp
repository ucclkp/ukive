// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "title_bar.h"

#include "ukive/window/window.h"


namespace ukive {

    TitleBar::TitleBar(Context c)
        : TitleBar(c, {}) {}

    TitleBar::TitleBar(Context c, AttrsRef attrs)
        : super(c, attrs) {}

    TitleBar::~TitleBar() {}

    void TitleBar::onAttachedToWindow(Window* w) {
        super::onAttachedToWindow(w);
        w->addStatusChangedListener(this);
    }

    void TitleBar::onDetachFromWindow() {
        getWindow()->removeStatusChangedListener(this);
        super::onDetachFromWindow();
    }

}
