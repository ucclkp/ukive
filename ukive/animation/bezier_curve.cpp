// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "bezier_curve.h"

#include <cmath>


namespace ukive {

    BezierCurve::BezierCurve(
        double V, double T, const Point& p1, const Point& p2)
    {
        for (int i = 0; i <= kSegCount; ++i) {
            double s = i / double(kSegCount);
            points_[i].t = 3 * p1.t*s*(1 - s)*(1 - s) + 3 * p2.t*s*s*(1 - s) + T * s*s*s;
            points_[i].v = V * std::pow(1 - s, 3) + 3 * p1.v*s*(1 - s)*(1 - s) + 3 * p2.v*s*s*(1 - s);
        }
    }

    bool BezierCurve::cal(double t, double scale, double* v) {
        for (int i = 0; i < kSegCount; ++i) {
            auto& cur = points_[i];
            auto& next = points_[i + 1];
            double cur_t = cur.t * std::abs(scale);
            double next_t = next.t * std::abs(scale);
            if (t >= cur_t && t < next_t) {
                double cur_v = (t - cur_t) / (next_t - cur_t) * (next.v - cur.v) + cur.v;
                *v = cur_v * scale;
                return true;
            }
        }
        return false;
    }

}
