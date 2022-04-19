// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/path_mac.h"

#import <Cocoa/Cocoa.h>


namespace ukive {
namespace mac {

    bool PathMac::open() {
        path_ = [NSBezierPath bezierPath];
        return path_ != nullptr;
    }

    void PathMac::begin(const PointF &start, bool fill) {
        [path_ moveToPoint:NSMakePoint(start.x(), start.y())];
    }

    void PathMac::end(bool close) {
        if (close) {
            [path_ closePath];
        }
    }

    bool PathMac::close() {
        return true;
    }

    void PathMac::addLine(const PointF &p) {
        [path_ lineToPoint:NSMakePoint(p.x(), p.y())];
    }

    void PathMac::addQBezier(const PointF &p1, const PointF &p2) {
        auto qp0 = [path_ currentPoint];

        auto cp1 = NSMakePoint(qp0.x + 2.f / 3 * (p1.x() - qp0.x), qp0.y + 2.f / 3 * (p1.y() - qp0.y));
        auto cp2 = NSMakePoint(p2.x() + 2.f / 3 * (p1.x() - p2.x()), p2.y() + 2.f / 3 * (p1.y() - p2.y()));
        auto cp3 = NSMakePoint(p2.x(), p2.y());

        [path_ curveToPoint:cp1 controlPoint1:cp2 controlPoint2:cp3];
    }

    NSBezierPath* PathMac::getNative() const {
        return path_;
    }

}
}
