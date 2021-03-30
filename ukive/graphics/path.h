// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_PATH_H_
#define UKIVE_GRAPHICS_PATH_H_

#include "ukive/graphics/point.hpp"


namespace ukive {

    class Path {
    public:
        static Path* create();

        virtual ~Path() = default;

        virtual bool open() = 0;
        virtual void begin(const PointF& start, bool fill) = 0;
        virtual void end(bool close) = 0;
        virtual bool close() = 0;

        virtual void addLine(const PointF& p) = 0;
        virtual void addQBezier(const PointF& p1, const PointF& p2) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_PATH_H_