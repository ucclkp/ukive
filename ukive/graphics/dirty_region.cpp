// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "dirty_region.h"


namespace ukive {

    DirtyRegion::DirtyRegion() {}

    void DirtyRegion::add(const Rect& rect) {
        if (rect.empty()) {
            return;
        }

        if (rect0.empty()) {
            rect0 = rect;
            return;
        }

        if (rect0.intersect(rect)) {
            rect0.join(rect);

            if (!rect1.empty() && rect0.intersect(rect1)) {
                rect0.join(rect1);
                rect1.set(0, 0, 0, 0);
            }
            return;
        }

        if (rect1.empty()) {
            rect1 = rect;
            return;
        }

        if (rect1.intersect(rect)) {
            rect1.join(rect);

            if (rect0.intersect(rect1)) {
                rect0.join(rect1);
                rect1.set(0, 0, 0, 0);
            }
            return;
        }

        rect0.join(rect);
        if (rect0.intersect(rect1)) {
            rect0.join(rect1);
            rect1.set(0, 0, 0, 0);
        }
    }

    void DirtyRegion::setOne(const Rect& rect) {
        rect0 = rect;
        rect1.set(0, 0, 0, 0);
    }

    void DirtyRegion::clear() {
        rect0.set(0, 0, 0, 0);
        rect1.set(0, 0, 0, 0);
    }

    bool DirtyRegion::empty() const {
        return rect0.empty() && rect1.empty();
    }

}
