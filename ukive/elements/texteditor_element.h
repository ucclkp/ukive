// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_TEXTEDITOR_ELEMENT_H_
#define UKIVE_ELEMENTS_TEXTEDITOR_ELEMENT_H_

#include "ukive/elements/element.h"
#include "ukive/graphics/color.h"
#include "ukive/window/context.h"


namespace ukive {

    class Window;

    class TextEditorElement : public Element {
    public:
        explicit TextEditorElement(Context c);
        ~TextEditorElement();

        bool onFocusChanged(bool focus) override;
        void draw(Canvas *canvas) override;
        Opacity getOpacity() const override;

    private:
        Context context_;

        int panel_line_width_;
        Color panel_line_color_;
    };

}

#endif  // UKIVE_ELEMENTS_TEXTEDITOR_ELEMENT_H_