// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_OVERLAY_SCROLL_BAR_H_
#define UKIVE_VIEWS_LIST_OVERLAY_SCROLL_BAR_H_

#include <functional>

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Canvas;

    class OverlayScrollBar {
    public:
        OverlayScrollBar();
        ~OverlayScrollBar();

        void update(int content_height, float percent);

        void registerScrollHandler(std::function<void(int)> h);

        void setBounds(int x, int y, int width, int height);
        void setBounds(const Rect& bounds);

        void setScrollBarWidth(int width);
        void setScrollBarMinHeight(int min_height);

        void onDraw(Canvas* canvas);
        bool onMousePressed(const Point& p);
        bool onMouseDragged(const Point& p);

        bool isInScrollBar(const Point& p) const;

    private:
        void moveScroller(int distance_y);

        int content_height_;
        int scrollbar_width_;
        int scrollbar_min_height_;

        Rect view_bounds_;
        Rect scrollbar_bounds_;
        Rect thumb_bounds_;
        Rect thumb_bounds_start_;

        Point dragging_prev_;
        Point dragging_start_;

        std::function<void(int)> scroll_handler_;
    };

}

#endif  // UKIVE_VIEWS_LIST_OVERLAY_SCROLL_BAR_H_