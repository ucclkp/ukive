// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "overlay_scroll_bar.h"

#include <algorithm>

#include "ukive/graphics/canvas.h"


namespace ukive {

    OverlayScrollBar::OverlayScrollBar()
        : content_height_(0),
          scrollbar_width_(0),
          scrollbar_min_height_(0) {
    }

    OverlayScrollBar::~OverlayScrollBar() {}

    void OverlayScrollBar::update(int content_height, float percent) {
        content_height_ = content_height;

        int view_height = view_bounds_.height();
        if (view_height >= content_height_) {
            thumb_bounds_.xywh(0, 0, 0, 0);
            return;
        }

        if (content_height_ <= 0 || view_bounds_.empty()) {
            return;
        }

        float scale = view_height / static_cast<float>(content_height_);

        int thumb_width = scrollbar_bounds_.width();
        int thumb_height = (std::max)(static_cast<int>(view_height * scale), scrollbar_min_height_);
        int thumb_y = int(scrollbar_bounds_.y() + percent * (view_height - thumb_height));

        thumb_bounds_ = Rect(
            view_bounds_.right() - thumb_width,
            thumb_y,
            thumb_width, thumb_height);
    }

    void OverlayScrollBar::registerScrollHandler(std::function<void(int)> h) {
        scroll_handler_ = h;
    }

    void OverlayScrollBar::setBounds(int x, int y, int width, int height) {
        view_bounds_.xywh(x, y, width, height);
        scrollbar_bounds_.xywh(
            x + width - scrollbar_width_, y, scrollbar_width_, height);
    }

    void OverlayScrollBar::setBounds(const Rect& bounds) {
        view_bounds_ = bounds;
        scrollbar_bounds_.xywh(
            bounds.right() - scrollbar_width_,
            bounds.y(),
            scrollbar_width_, bounds.height());
    }

    void OverlayScrollBar::setScrollBarWidth(int width) {
        if (width > 0) {
            scrollbar_width_ = width;
        }
    }

    void OverlayScrollBar::setScrollBarMinHeight(int min_height) {
        if (min_height > 0) {
            scrollbar_min_height_ = min_height;
        }
    }

    void OverlayScrollBar::onDraw(Canvas* canvas) {
        if (!thumb_bounds_.empty()) {
            canvas->fillRect(RectF(thumb_bounds_), Color::Grey400);
        }
    }

    bool OverlayScrollBar::onMousePressed(const Point& p) {
        if (thumb_bounds_.empty()) {
            return false;
        }

        if (scrollbar_bounds_.hit(p)) {
            thumb_bounds_start_ = thumb_bounds_;

            if (thumb_bounds_.hit(p)) {
                dragging_start_ = p;
            } else {
                int distance_y = 0;
                if (p.y() < thumb_bounds_.y()) {
                    distance_y = -thumb_bounds_.height();
                } else if (p.y() >= thumb_bounds_.bottom()) {
                    distance_y = thumb_bounds_.height();
                }

                if (distance_y != 0) {
                    moveScroller(distance_y);
                }
            }

            return true;
        }
        return false;
    }

    bool OverlayScrollBar::onMouseDragged(const Point& p) {
        int distance_y = p.y() - dragging_start_.y();
        if (distance_y != 0) {
            moveScroller(distance_y);
        }
        return true;
    }

    bool OverlayScrollBar::isInScrollBar(const Point& p) const {
        if (thumb_bounds_.empty()) {
            return false;
        }
        return scrollbar_bounds_.hit(p);
    }

    void OverlayScrollBar::moveScroller(int distance_y) {
        int view_height = view_bounds_.height();

        int thumb_y = (std::max)(0 - thumb_bounds_start_.y(), distance_y);
        thumb_y = (std::min)(view_height - thumb_bounds_start_.height() - thumb_bounds_start_.y(), thumb_y);

        thumb_bounds_ = thumb_bounds_start_;
        thumb_bounds_.offset(0, thumb_y);

        float scale = thumb_bounds_.y() / static_cast<float>(view_height - thumb_bounds_.height());
        int to_y = int((content_height_ - view_height) * scale);

        if (scroll_handler_) {
            scroll_handler_(to_y);
        }
    }

}