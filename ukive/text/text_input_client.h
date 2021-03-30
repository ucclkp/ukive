// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_TEXT_INPUT_CLIENT_H_
#define UKIVE_TEXT_TEXT_INPUT_CLIENT_H_

#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Editable;
    class Window;

    class TextInputClient {
    public:
        virtual ~TextInputClient() = default;

        virtual void onTICBeginProcess() = 0;
        virtual void onTICEndProcess() = 0;
        virtual void onTICRedrawSelection() = 0;

        virtual bool isTICEditable() const = 0;
        virtual Editable* getTICEditable() const = 0;
        virtual Window* getTICHostWindow() const = 0;
        virtual void getTICBounds(Rect* bounds) const = 0;
        virtual void getTICSelectionBounds(
            size_t sel_start, size_t sel_end, Rect* bounds, bool* clipped) const = 0;
    };

}

#endif  // UKIVE_TEXT_TEXT_INPUT_CLIENT_H_