// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_DIAGNOSTIC_GRID_NAVIGATOR_H_
#define UKIVE_DIAGNOSTIC_GRID_NAVIGATOR_H_

#include "ukive/menu/inner_window.h"


namespace ukive {

    class View;

    class GridNavigator :
        public OnInnerWindowEventListener
    {
    public:
        explicit GridNavigator(View* v);

        void create(
            const std::u16string& text,
            const Color& text_color,
            const Color& bg_color);

        void showNav(const Rect& rect);
        void closeNav();

        // OnInnerWindowEventListener
        void onBeforeInnerWindowLayout(
            InnerWindow* iw, Rect* new_bounds, const Rect& old_bounds) override;

    private:
        void show(int x, int y);
        void close();

        View* parent_view_;

        bool is_finished_ = true;
        std::shared_ptr<InnerWindow> inner_window_;
    };

}

#endif  // UKIVE_DIAGNOSTIC_GRID_NAVIGATOR_H_