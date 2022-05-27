// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ELEMENTS_CHESSBOARD_ELEMENT_H_
#define UKIVE_ELEMENTS_CHESSBOARD_ELEMENT_H_

#include "ukive/elements/element.h"
#include "ukive/graphics/colors/color.h"
#include "ukive/window/context.h"


namespace ukive {

    class ChessboardElement : public Element {
    public:
        explicit ChessboardElement(Context c);

    protected:
        void draw(Canvas* canvas) override;
        bool isTransparent() const override;

    private:
        Context context_;

        int cell_length_;
        Color dark_color_;
        Color light_color_;
    };

}

#endif  // UKIVE_ELEMENTS_CHESSBOARD_ELEMENT_H_