// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_CHART_VIEW_H_
#define UKIVE_VIEWS_CHART_VIEW_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class ChartView : public View {
    public:
        struct Node {
            double x;
            double y;
            bool draw_point;
            bool single_point;
        };

        explicit ChartView(Context c);
        ChartView(Context c, AttrsRef attrs);

        void addData(
            double x, double y,
            bool draw_point = true, bool single_point = false);
        void clear();

        const std::vector<Node>& getData() const {
            return data_;
        }

    protected:
        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* canvas) override;

    private:
        std::vector<Node> data_;
    };

}

#endif  // UKIVE_VIEWS_CHART_VIEW_H_