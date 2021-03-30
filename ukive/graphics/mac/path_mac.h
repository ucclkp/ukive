// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_PATH_MAC_H_
#define UKIVE_GRAPHICS_MAC_PATH_MAC_H_

#include "ukive/graphics/path.h"

#include "utils/mac/objc_utils.hpp"

UTL_OBJC_CLASS(NSBezierPath);


namespace ukive {

    class PathMac : public Path {
    public:
        PathMac() = default;

        bool open() override;
        void begin(const PointF &start, bool fill) override;
        void end(bool close) override;
        bool close() override;

        void addLine(const PointF &p) override;
        void addQBezier(const PointF &p1, const PointF &p2) override;

        NSBezierPath* getNative() const;

    private:
        NSBezierPath* path_ = nullptr;
    };

}

#endif  // UKIVE_GRAPHICS_MAC_PATH_MAC_H_
