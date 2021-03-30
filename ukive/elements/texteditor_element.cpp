// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "texteditor_element.h"

#include <cmath>

#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"


namespace ukive {

    TextEditorElement::TextEditorElement(Context c)
        : Element(),
          context_(c),
          panel_line_color_(Color::Grey400)
    {
        panel_line_width_ = int(std::ceil(c.dp2px(1.5f)));
    }

    TextEditorElement::~TextEditorElement() {}


    bool TextEditorElement::onFocusChanged(bool focus) {
        if (focus) {
            //panel_line_width_ = 2;
            panel_line_color_ = Color::Blue500;
        } else {
            //panel_line_width_ = 2;
            panel_line_color_ = Color::Grey400;
        }

        return true;
    }

    void TextEditorElement::draw(Canvas* canvas) {
        auto bound = getBounds();

        canvas->fillRect(
            RectF(
                bound.left, bound.bottom - panel_line_width_,
                bound.width(), panel_line_width_),
            panel_line_color_);
    }

    Element::Opacity TextEditorElement::getOpacity() const {
        return OPA_OPAQUE;
    }

}
