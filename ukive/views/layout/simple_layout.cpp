// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "simple_layout.h"


namespace ukive {

    SimpleLayout::SimpleLayout(Context c)
        : SimpleLayout(c, {}) {}

    SimpleLayout::SimpleLayout(Context c, AttrsRef attrs)
        : LayoutView(c, attrs) {}

    Size SimpleLayout::onDetermineSize(const SizeInfo& info) {
        determineChildrenSize(info);
        return getWrappedSize(info);
    }

    void SimpleLayout::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& size = child->getDeterminedSize();

                int child_left = getPadding().start + child->getLayoutMargin().start;
                int child_top = getPadding().top + child->getLayoutMargin().top;

                child->layout(
                    Rect(child_left, child_top, size.width, size.height));
            }
        }
    }

}