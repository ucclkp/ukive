// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shade_layout.h"


namespace ukive {

    ShadeLayout::ShadeLayout(Context c)
        : ShadeLayout(c, {}) {}

    ShadeLayout::ShadeLayout(Context c, AttrsRef attrs)
        : super(c, attrs) {}

    LayoutInfo* ShadeLayout::makeExtraLayoutInfo() const {
        return new ShadeLayoutInfo();
    }

    LayoutInfo* ShadeLayout::makeExtraLayoutInfo(AttrsRef attrs) const {
        return new ShadeLayoutInfo();
    }

    bool ShadeLayout::isValidExtraLayoutInfo(LayoutInfo* lp) const {
        return typeid(*lp) == typeid(ShadeLayoutInfo);
    }

    void ShadeLayout::onLayout(
        const Rect& new_bounds,
        const Rect& old_bounds)
    {
        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& size = child->getDeterminedSize();
                auto li = static_cast<ShadeLayoutInfo*>(child->getExtraLayoutInfo());
                if (li->gravity == GV_NONE) {
                    int child_left = getPadding().start + child->getLayoutMargin().start;
                    int child_top = getPadding().top + child->getLayoutMargin().top;

                    child->layout(
                        Rect(child_left, child_top, size.width, size.height));
                } else {
                    Rect bounds;
                    calculateGravityBounds(
                        getBoundsInRoot(), li->is_max_visible, li->is_discretized,
                        li->anchor, li->gravity, size, &bounds, &li->adj_gravity);
                    // li->anchor 是 RootLayout 中的坐标，计算出的 bounds 也一样。
                    // 需要转换到 ShadeLayout 中的坐标。
                    bounds.offset(-getLeft(), -getTop());
                    child->layout(bounds);
                }
            }
        }
    }

}
