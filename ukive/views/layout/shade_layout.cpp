// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shade_layout.h"

#include "ukive/views/title_bar/title_bar.h"
#include "ukive/window/window.h"


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
        return super::onDetermineSize(info);
    }

    void ShadeLayout::onLayout(
        const Rect& new_bounds,
        const Rect& old_bounds)
    {
        int title_bar_height = 0;
        auto title_bar = getWindow()->getTitleBar();
        if (title_bar) {
            title_bar_height = title_bar->getHeight();
        }

        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& size = child->getDeterminedSize();
                auto li = static_cast<ShadeLayoutInfo*>(child->getExtraLayoutInfo());
                auto& params = li->params;

                auto root_bounds = getBoundsInRoot();
                root_bounds.insets(getPadding() + child->getLayoutMargin());
                root_bounds.insets(0, title_bar_height, 0, 0);

                Rect bounds;
                if (params.type == ShadeParams::LT_POS) {
                    if (params.limit.valid()) {
                        root_bounds = params.limit.get();
                        root_bounds.insets(child->getLayoutMargin());
                    }

                    Point pos = params.pos;
                    if (params.relative) {
                        auto br = params.relative->getBoundsInRoot();
                        pos.x() += br.x();
                        pos.y() += br.y();
                    }

                    calculateCornerBounds(
                        root_bounds, params.is_max_visible, params.is_evaded,
                        pos, params.pos_padding, params.gravity, size, &bounds, &li->adj_corner);
                } else {
                    calculateGravityBounds(
                        root_bounds, params.is_max_visible, params.is_discretized,
                        li->params.anchor.get(), params.gravity, size, &bounds, &li->adj_gravity);
                }

                bounds.same(root_bounds);
                // li->anchor 是 RootLayout 中的坐标，计算出的 bounds 也一样。
                // 需要转换到 ShadeLayout 中的坐标。
                bounds.offset(-getX(), -getY());
                child->layout(bounds);
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
