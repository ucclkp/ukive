// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/elements/chessboard_element.h"

#include <algorithm>

#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"


namespace {

    const int kCellLength = 16;
    const ukive::Color kLightColor = ukive::Color::White;
    const ukive::Color kDarkColor = ukive::Color::Grey100;
    //const ukive::Color kLightColor = ukive::Color::Grey900;
    //const ukive::Color kDarkColor = ukive::Color::Black;

}

namespace ukive {

    ChessboardElement::ChessboardElement(Context c)
        : context_(c),
          cell_length_(c.dp2pxi(kCellLength)),
          dark_color_(kDarkColor),
          light_color_(kLightColor) {
    }

    void ChessboardElement::draw(Canvas* canvas) {
        auto bounds = getBounds();
        if (bounds.empty()) {
            return;
        }

        int cur_left = bounds.left;
        int cur_top = bounds.top;
        bool is_dark_color = false;
        bool is_vert_dark_color = !is_dark_color;
        for (;;) {
            for (;;) {
                int cur_width = (std::min)(cell_length_, bounds.right - cur_left);
                int cur_height = (std::min)(cell_length_, bounds.bottom - cur_top);
                if (cur_width <= 0 || cur_height <= 0) {
                    break;
                }

                Rect cell(cur_left, cur_top, cur_width, cur_height);
                canvas->fillRect(RectF(cell), is_dark_color ? dark_color_ : light_color_);
                is_dark_color = !is_dark_color;
                cur_left += cell_length_;
                if (cur_left >= bounds.right) {
                    break;
                }
            }

            cur_left = bounds.left;
            is_dark_color = is_vert_dark_color;
            is_vert_dark_color = !is_vert_dark_color;
            cur_top += cell_length_;
            if (cur_top >= bounds.bottom) {
                break;
            }
        }
    }

}