// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_ANIMATION_BEZIER_CURVE_H_
#define UKIVE_ANIMATION_BEZIER_CURVE_H_

#include <cstddef>


namespace ukive {

    /**
     * 一条三次贝塞尔曲线的近似描述。
     * 使用 kSegCount + 1 个采样点对参数方程等间隔采样得到。
     * 该曲线位于横轴为时间(t)，纵轴为速度(v)的二维直角坐标系中。
     * 曲线开始于纵轴上的某一点，终止于横轴上的某一点。
     */
    class BezierCurve {
    public:
        struct Point {
            double t, v;
        };
        const static size_t kSegCount = 100;

        /**
         * 构造曲线。
         * @param V 对应于起始控制点 p0(0, V)
         * @param T 对应于结束控制点 p3(T, 0)
         * @param p1 对应于控制点 p1
         * @param p2 对应于控制点 p2
         */
        BezierCurve(double V, double T, const Point& p1, const Point& p2);

        /**
         * 由时间值计算曲线上对应的速度值。
         * @param t 横轴上的某一位置，即时间值
         * @param scale 坐标的缩放值，横轴和纵轴均使用该值
         * @param v 横轴上 t 位置对应于曲线上的纵坐标值，即速度值
         * @return 若位置 t 有对应的曲线点，返回 true，否则返回 false。
         */
        bool cal(double t, double scale, double* v);

    private:
        Point points_[kSegCount + 1];
    };

}

#endif  // UKIVE_ANIMATION_BEZIER_CURVE_H_
