// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "tooltip.h"

#include "ukive/elements/element.h"
#include "ukive/views/text_view.h"


namespace ukive {

    Tooltip::Tooltip(Context c) {
        auto element = new Element(Element::SHAPE_RRECT, Color::White);
        auto view = new TextView(c);
        view->setText(u"test");
        view->setPadding(c.dp2pxi(6), c.dp2pxi(4), c.dp2pxi(6), c.dp2pxi(4));

        levitator_.setBackground(element);
        levitator_.setOutsideTouchable(true);
        levitator_.setShadowRadius(c.dp2pxi(4));
        levitator_.setContentView(view);
    }

    void Tooltip::show(View* anchor, int gravity) {
        levitator_.show(anchor, gravity);
    }

    void Tooltip::show(View* host, int x, int y) {
        levitator_.show(host->getWindow(), x, y);
    }

    void Tooltip::close() {
        levitator_.dismiss();
    }

    void Tooltip::setText(const std::u16string_view& text) {

    }

    void Tooltip::setTimeout(utl::TimeUtils::nsp timeout) {

    }

}