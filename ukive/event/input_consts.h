// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_EVENT_INPUT_CONSTS_H_
#define UKIVE_EVENT_INPUT_CONSTS_H_


namespace ukive {

    /**
     * 由 VelocityCalculator 计算出的速度值的最大限制。
     * 单位为像素/秒。
     */
    extern const double kMaxVelocityInCal;

    /**
     * 由 Scroller 使用的速度值的最大限制。
     * 单位为像素/秒。
     */
    extern const double kMaxVelocityInScroller;

    /**
     * 触摸滑动的阈值。
     * 低于此值的位移不会被当成是滑动手势。
     */
    extern const float kTouchScrollingThreshold;

    /**
     * 鼠标移动的阈值。
     * 低于此值的位移不会被当成是移动。
     */
    extern const float kMouseScrollingThreshold;

    /**
     * 鼠标拖动的阈值。
     * 低于此值的位移不会被当成是拖动。
     */
    extern const float kMouseDraggingThreshold;

}

#endif  // UKIVE_EVENT_INPUT_CONSTS_H_