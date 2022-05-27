// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TITLE_BAR_CIRCLE_COLOR_BUTTON_H_
#define UKIVE_VIEWS_TITLE_BAR_CIRCLE_COLOR_BUTTON_H_

#include "ukive/views/view.h"


namespace ukive {

    class Color;
    class Element;
    class RippleElement;

    class CircleColorButton : public View {
    public:
        explicit CircleColorButton(Context c);
        CircleColorButton(Context c, AttrsRef attrs);

        void setSize(int size);
        void setColor(const Color& c);

    protected:
        // View
        Size onDetermineSize(const SizeInfo& info) override;

    private:
        int size_;
        RippleElement* bg_;
    };

}

#endif  // UKIVE_VIEWS_TITLE_BAR_CIRCLE_COLOR_BUTTON_H_