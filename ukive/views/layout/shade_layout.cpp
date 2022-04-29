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

    Size ShadeLayout::onDetermineSize(const SizeInfo& info) {
        auto size = super::onDetermineSize(info);
        return size;
    }

    void ShadeLayout::onLayout(
        const Rect& new_bounds,
        const Rect& old_bounds)
    {
        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& size = child->getDeterminedSize();
                auto li = static_cast<ShadeLayoutInfo*>(child->getExtraLayoutInfo());
                auto cp = getPadding() + child->getLayoutMargin();

                if (li->gravity == GV_NONE) {
                    child->layout(
                        Rect(cp.start(), cp.top(), size.width(), size.height()));
                } else {
                    auto root_bounds = getBoundsInRoot();
                    root_bounds.insets(cp);

                    auto anchor_bounds = li->anchor;
                    anchor_bounds.same(root_bounds);

                    Rect bounds;
                    calculateGravityBounds(
                        root_bounds, li->is_max_visible, li->is_discretized,
                        anchor_bounds, li->gravity, size, &bounds, &li->adj_gravity);

                    bounds.same(root_bounds);

                    // li->anchor 是 RootLayout 中的坐标，计算出的 bounds 也一样。
                    // 需要转换到 ShadeLayout 中的坐标。
                    bounds.offset(-getX(), -getY());
                    child->layout(bounds);
                }
            }
        }
    }

    bool ShadeLayout::hitChildren(int x, int y) {
        for (auto c : *this) {
            if (c->getBounds().hit(x, y)) {
                return true;
            }
        }
        return false;
    }

}
