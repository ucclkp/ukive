// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_H_
#define UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_H_

#include "ukive/views/layout/simple_layout.h"
#include "ukive/window/window_listener.h"


namespace ukive {

    class TitleBar :
        public SimpleLayout,
        public OnWindowStatusChangedListener {
    public:
        explicit TitleBar(Context c);
        TitleBar(Context c, AttrsRef attrs);
        ~TitleBar();

        void onAttachedToWindow(Window* w) override;
        void onDetachFromWindow() override;

        virtual HitPoint onNCHitTest(int x, int y) = 0;

    private:
        using super = SimpleLayout;
    };

}

#endif  // UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_H_