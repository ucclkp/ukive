// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/title_bar/circle_color_button.h"

#include <algorithm>

#include "ukive/elements/ripple_element.h"
#include "ukive/elements/shape_element.h"
#include "ukive/window/window.h"


namespace ukive {

    CircleColorButton::CircleColorButton(Context c)
        : CircleColorButton(c, {}) {}

    CircleColorButton::CircleColorButton(Context c, AttrsRef attrs)
        : View(c, attrs)
    {
        setOutline(OUTLINE_OVAL);
        setClickable(true);

        shape_element_ = new ShapeElement(ShapeElement::OVAL);
        shape_element_->setSolidEnable(true);
        shape_element_->setSolidColor(Color::White);

        ripple_background_ = new RippleElement();
        ripple_background_->add(shape_element_);

        setBackground(ripple_background_);
        setShadowRadius(c.dp2pxi(2.0f));

        size_ = c.dp2pxi(18);
    }

    void CircleColorButton::setSize(int size) {
        if (size != size_) {
            size_ = size;
            requestLayout();
            requestDraw();
        }
    }

    void CircleColorButton::setColor(const Color& c) {
        shape_element_->setSolidColor(c);
        requestDraw();
    }

    Size CircleColorButton::onDetermineSize(const SizeInfo& info) {
        int final_w, final_h;
        switch (info.width.mode) {
        case SizeInfo::CONTENT:
            final_w = std::min(info.width.val, size_ + getPadding().hori());
            break;

        case SizeInfo::DEFINED:
            final_w = info.width.val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_w = size_ + getPadding().hori();
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT:
            final_h = std::min(info.height.val, size_ + getPadding().vert());
            break;

        case SizeInfo::DEFINED:
            final_h = info.height.val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_h = size_ + getPadding().vert();
            break;
        }

        int size = std::min(final_w, final_h);
        return Size(size, size);
    }

}
