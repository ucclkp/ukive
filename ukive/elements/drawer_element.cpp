// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "drawer_element.h"


namespace ukive {

    DrawerElement::DrawerElement(const Func& f)
        : drawer_(f) {}

    void DrawerElement::draw(Canvas* canvas) {
        if (drawer_) {
            drawer_(canvas, getBounds(), getState(), isParentHasFocus());
        }
    }

}
