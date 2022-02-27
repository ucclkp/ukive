// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_DRAWER_ELEMENT_H_
#define UKIVE_ELEMENTS_DRAWER_ELEMENT_H_

#include <functional>

#include "ukive/elements/element.h"
#include "ukive/graphics/canvas.h"


namespace ukive {

    class DrawerElement : public Element {
    public:
        using Func = std::function<
            void(Canvas* c, const Rect& bounds, int state, bool focus)>;

        explicit DrawerElement(const Func& f);

    protected:
        void draw(Canvas* canvas) override;

    private:
        Func drawer_;
    };

}

#endif  // UKIVE_ELEMENTS_DRAWER_ELEMENT_H_