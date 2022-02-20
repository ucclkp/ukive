// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/chart_view.h"

#include <algorithm>
#include <cmath>

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/point.hpp"
#include "ukive/window/window.h"


namespace ukive {

    ChartView::ChartView(Context c)
        : ChartView(c, {}) {}

    ChartView::ChartView(Context c, AttrsRef attrs)
        : View(c, attrs) {}

    Size ChartView::onDetermineSize(const SizeInfo& info) {
        return getPreferredSize(info, 0, 0);
    }

    void ChartView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        int node_radius = int(std::round(getContext().dp2px(3)));
        auto bounds = getContentBounds();
        auto width = bounds.width() - node_radius * 2;
        auto height = bounds.height() - node_radius * 2;
        if (width <= 0 || height <= 0) {
            return;
        }

        double left = (std::numeric_limits<double>::max)();
        double right = std::numeric_limits<double>::lowest();
        double top = (std::numeric_limits<double>::max)();
        double bottom = std::numeric_limits<double>::lowest();
        for (const auto& d : data_) {
            left = (std::min)(left, d.x);
            right = (std::max)(right, d.x);
            top = (std::min)(top, d.y);
            bottom = (std::max)(bottom, d.y);
        }

        double dw = right - left;
        double dh = bottom - top;
        if (dw <= 0 || dh <= 0) {
            return;
        }

        Point prev;
        bool has_prev = false;
        for (const auto& d : data_) {
            int vx = int(node_radius + width / dw * (d.x - left));
            int vy = int(node_radius + height / dh * (d.y - top));
            vy = bounds.height() - vy;

            if (d.draw_point) {
                canvas->fillCircle(
                    PointF(Point(vx, vy)), float(node_radius), Color::Blue300);
            }
            if (has_prev && !d.single_point) {
                canvas->drawLine(
                    PointF(prev),
                    PointF(Point(vx, vy)),
                    getContext().dp2px(1.5f), Color::Blue300);
            }

            if (!d.single_point) {
                prev.set(vx, vy);
                has_prev = true;
            }
        }
    }

    void ChartView::addData(
        double x, double y, bool draw_point, bool single_point)
    {
        data_.push_back({ x, y, draw_point, single_point });
        requestDraw();
    }

    void ChartView::clear() {
        data_.clear();
        requestDraw();
    }

}
