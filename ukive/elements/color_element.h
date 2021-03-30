// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_COLOR_ELEMENT_H_
#define UKIVE_ELEMENTS_COLOR_ELEMENT_H_

#include "ukive/elements/element.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class ColorElement : public Element {
    public:
        explicit ColorElement(Color color);
        ~ColorElement();

        void draw(Canvas *canvas) override;
        Opacity getOpacity() const override;

        void setColor(const Color& c);
        Color getColor() const;

    private:
        Color color_;
    };

}

#endif  // UKIVE_ELEMENTS_COLOR_ELEMENT_H_