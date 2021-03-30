// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/resources/dimension.h"

#include "ukive/window/context.h"


namespace ukive {

    Dimension::Dimension()
        : px(0), type(PIXEL) {}

    Dimension::Dimension(int px)
        : px(px), type(PIXEL) {}

    Dimension::Dimension(float dp)
        : dp(dp), type(DIP) {}

    Dimension& Dimension::operator=(int px) {
        this->px = px;
        type = PIXEL;
        return *this;
    }
    Dimension& Dimension::operator=(float dp) {
        this->dp = dp;
        type = DIP;
        return *this;
    }

    void Dimension::setPx(int px) {
        this->px = px;
        type = PIXEL;
    }

    void Dimension::setDp(float dp) {
        this->dp = dp;
        type = DIP;
    }

    int Dimension::toPxX(const Context& c) const {
        if (type == DIP) {
            return static_cast<int>(c.dp2px(dp));
        }
        return px;
    }

    int Dimension::toPxY(const Context& c) const {
        if (type == DIP) {
            return static_cast<int>(c.dp2px(dp));
        }
        return px;
    }

    bool Dimension::isDp() const {
        return type == DIP;
    }

    bool Dimension::isPx() const {
        return type == PIXEL;
    }

}
