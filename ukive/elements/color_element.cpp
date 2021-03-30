// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "color_element.h"

#include "ukive/graphics/canvas.h"


namespace ukive {

    ColorElement::ColorElement(Color color)
        : Element(),
          color_(color) {
    }

    ColorElement::~ColorElement() {
    }

    void ColorElement::draw(Canvas *canvas) {
        canvas->fillRect(RectF(getBounds()), color_);
    }

    Element::Opacity ColorElement::getOpacity() const {
        if (color_.a >= 1.0f) {
            return OPA_OPAQUE;
        }
        if (color_.a == 0) {
            return OPA_TRANSPARENT;
        }
        return OPA_SEMILUCENT;
    }

    void ColorElement::setColor(const Color& c) {
        color_ = c;
    }

    Color ColorElement::getColor() const {
        return color_;
    }

}