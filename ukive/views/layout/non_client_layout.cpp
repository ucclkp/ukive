// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/layout/non_client_layout.h"

#include <algorithm>

#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"


namespace ukive {

    Color border_color = Color::Blue500;

    NonClientLayout::NonClientLayout(Context c)
        : NonClientLayout(c, {}) {}

    NonClientLayout::NonClientLayout(Context c, AttrsRef attrs)
        : LayoutView(c, attrs)
    {}

    HitPoint NonClientLayout::onNCHitTest(int x, int y) {
        if (getWindow()->isMaximized()) {
            return HitPoint::CLIENT;
        }

        int row = 1, col = 1;

        if (x >= 0 && x < sh_padding_.start()) {
            col = 0;
        } else if (x >= getWidth() - sh_padding_.end() && x < getWidth()) {
            col = 2;
        }

        if (y >= 0 && y < sh_padding_.top()) {
            row = 0;
        } else if (y >= getHeight() - sh_padding_.bottom() && y < getHeight()) {
            row = 2;
        }

        static HitPoint hitTests[3][3]{
            { HitPoint::TOP_LEFT,    HitPoint::TOP,    HitPoint::TOP_RIGHT    },
            { HitPoint::LEFT,        HitPoint::CLIENT, HitPoint::RIGHT        },
            { HitPoint::BOTTOM_LEFT, HitPoint::BOTTOM, HitPoint::BOTTOM_RIGHT },
        };

        return hitTests[row][col];
    }

    void NonClientLayout::setNonClientPadding(int left, int top, int right, int bottom) {
        nc_padding_.set(left, top, right - left, bottom - top);
        requestLayout();
        requestDraw();
    }

    void NonClientLayout::setSizeHandlePadding(int left, int top, int right, int bottom) {
        sh_padding_.set(left, top, right - left, bottom - top);
        requestLayout();
        requestDraw();
    }

    Size NonClientLayout::onDetermineSize(const SizeInfo& info) {
        int final_width;
        int final_height;

        int nc_hori_padding = nc_padding_.hori();
        int nc_vert_padding = nc_padding_.vert();

        int hori_padding = getPadding().hori() + nc_hori_padding;
        int vert_padding = getPadding().vert() + nc_vert_padding;

        auto new_info = info;
        new_info.width().val = info.width().val - nc_hori_padding;
        new_info.height().val = info.height().val - nc_vert_padding;

        determineChildrenSize(new_info);

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
            final_width = getWrappedWidth();
            final_width = (std::min)(final_width + hori_padding, info.width().val);
            break;

        case SizeInfo::FREEDOM:
            final_width = getWrappedWidth() + hori_padding;
            break;

        case SizeInfo::DEFINED:
        default:
            final_width = info.width().val;
            break;
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            final_height = getWrappedHeight();
            final_height = (std::min)(final_height + vert_padding, info.height().val);
            break;

        case SizeInfo::FREEDOM:
            final_height = getWrappedHeight() + vert_padding;
            break;

        case SizeInfo::DEFINED:
        default:
            final_height = info.height().val;
            break;
        }

        return Size(final_width, final_height);
    }

    void NonClientLayout::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        // layout 里面可能会添加子 View, 这里不要用迭代器
        for (size_t i = 0; i < getChildCount(); ++i) {
            auto child = getChildAt(i);
            if (child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();

                auto& size = child->getDeterminedSize();

                int child_left = getPadding().start() + nc_padding_.start() + margin.start();
                int child_top = getPadding().top() + nc_padding_.top() + margin.top();

                child->layout(
                    Rect(child_left, child_top, size.width(), size.height()));
            }
        }
    }

    void NonClientLayout::onDraw(Canvas* canvas) {
        if (!nc_padding_.empty()) {
            Rect left_rect(
                0, 0,
                nc_padding_.start(), getHeight() - nc_padding_.bottom());
            canvas->fillRect(RectF(left_rect), border_color);

            Rect top_rect(
                nc_padding_.start(), 0,
                getWidth() - nc_padding_.start(), nc_padding_.top());
            canvas->fillRect(RectF(top_rect), border_color);

            Rect right_rect(
                getWidth() - nc_padding_.end(), nc_padding_.top(),
                nc_padding_.end(), getHeight() - nc_padding_.top());
            canvas->fillRect(RectF(right_rect), border_color);

            Rect bottom_rect(
                0, getHeight() - nc_padding_.bottom(),
                getWidth() - nc_padding_.end(), nc_padding_.bottom());
            canvas->fillRect(RectF(bottom_rect), border_color);
        }

        LayoutView::onDraw(canvas);
    }

    void NonClientLayout::onAttachedToWindow(Window* w) {
        LayoutView::onAttachedToWindow(w);

        if (!w->hasSizeBorder()) {
            auto c = getContext();
            nc_padding_.set(c.dp2pxi(0), c.dp2pxi(0), c.dp2pxi(0), c.dp2pxi(0));
            sh_padding_.set(c.dp2pxi(4), c.dp2pxi(4), c.dp2pxi(4), c.dp2pxi(4));
        }
    }

}
