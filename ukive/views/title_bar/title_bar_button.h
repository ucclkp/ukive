// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_BUTTON_H_
#define UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_BUTTON_H_

#include "ukive/views/view.h"
#include "ukive/window/window_listener.h"


namespace ukive {

    class Color;
    class Element;
    class RippleElement;

    class TitleBarButton : public View {
    public:
        explicit TitleBarButton(Context c);
        TitleBarButton(Context c, AttrsRef attrs);

        void setGraphSize(int size);
        void setType(WindowButton btn);

    protected:
        // View
        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* c) override;

    private:
        using super = View;

        int graph_size_;
        int graph_stroke_;
        int restore_bias_;
        RippleElement* bg_;
        WindowButton btn_ = WindowButton::None;
    };

}

#endif  // UKIVE_VIEWS_TITLE_BAR_DRAWING_BUTTON_H_