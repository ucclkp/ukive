// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "tooltip.h"

#include "ukive/elements/element.h"
#include "ukive/views/text_view.h"
#include "ukive/system/ui_utils.h"
#include "ukive/window/window.h"


namespace ukive {

    Tooltip::Tooltip(Context c) {
        auto element = new Element(Element::SHAPE_RRECT, Color::White);
        element->setRadius(c.dp2px(2));

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

    void Tooltip::show(Window* host, int x, int y) {
        auto cursor_size = getCurrentCursorSize();
        auto c = host->getContext();
        auto cursor_w = c.px2dpci(cursor_size.width());
        auto cursor_h = c.px2dpci(cursor_size.height());

        Levitator::PosInfo info;
        info.corner = GV_TOP | GV_MID_START;
        info.is_evaded = true;
        info.pp.set(1, 1, 0, cursor_h);

        levitator_.show(host, x, y, info);
    }

    void Tooltip::close() {
        levitator_.dismiss();
    }

    void Tooltip::setText(const std::u16string_view& text) {

    }

    void Tooltip::setTimeout(utl::TimeUtils::nsp timeout) {

    }

}