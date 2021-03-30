// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/layout/title_bar_layout.h"

#include <algorithm>


namespace ukive {

    TitleBarLayout::TitleBarLayout(Context c)
        : TitleBarLayout(c, {}) {}

    TitleBarLayout::TitleBarLayout(Context c, AttrsRef attrs)
        : LayoutView(c, attrs) {}

    Size TitleBarLayout::onDetermineSize(const SizeInfo& info) {
        int final_width;
        int final_height;

        int wrapped_width, wrapped_height;
        measureChildren(info, &wrapped_width, &wrapped_height);

        switch (info.width.mode) {
        case SizeInfo::CONTENT:
            final_width = std::min(wrapped_width + getPadding().hori(), info.width.val);
            break;

        case SizeInfo::FREEDOM:
            final_width = wrapped_width + getPadding().hori();
            break;

        case SizeInfo::DEFINED:
        default:
            final_width = info.width.val;
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT:
            final_height = std::min(wrapped_height + getPadding().vert(), info.height.val);
            break;

        case SizeInfo::FREEDOM:
            final_height = wrapped_height + getPadding().vert();
            break;

        case SizeInfo::DEFINED:
        default:
            final_height = info.height.val;
            break;
        }

        return Size(final_width, final_height);
    }


    void TitleBarLayout::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        // 子 View 数量超过一个时，将最后一个认定为 Bar
        View* bar;
        int count = getChildCount();
        if (count > 1) {
            bar = getChildAt(count - 1);
            if (bar->getVisibility() == VANISHED) {
                bar = nullptr;
            }
        } else {
            bar = nullptr;
        }

        // 将 Bar 放到顶部
        int bar_bottom;
        if (bar) {
            auto& margin = bar->getLayoutMargin();

            auto& size = bar->getDeterminedSize();

            int child_left = getPadding().start + margin.start;
            int child_top = getPadding().top + margin.top;

            bar->layout(
                Rect(child_left, child_top, size.width, size.height));

            --count;
            bar_bottom = size.height + child_top;
        } else {
            bar_bottom = getPadding().top;
        }

        // 剩余 View
        for (int i = 0; i < count; ++i) {
            auto child = getChildAt(i);
            if (child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();

                auto& size = child->getDeterminedSize();

                int child_left = getPadding().start + margin.start;
                int child_top = bar_bottom + margin.top;

                child->layout(
                    Rect(child_left, child_top, size.width, size.height));
            }
        }
    }

    void TitleBarLayout::measureChildren(
        const SizeInfo& parent_info,
        int* wrapped_width, int* wrapped_height)
    {
        // 子 View 数量超过一个时，将最后一个认定为 Bar
        View* bar;
        int count = getChildCount();
        if (count > 1) {
            bar = getChildAt(count - 1);
            if (bar->getVisibility() != VANISHED) {
                --count;
            } else {
                bar = nullptr;
            }
        } else {
            bar = nullptr;
        }

        // 若有 Bar，先测量 Bar
        int bar_height;
        if (bar) {
            determineChildSize(bar, parent_info);

            auto& size = bar->getDeterminedSize();
            auto& margin = bar->getLayoutMargin();
            *wrapped_width = size.width + margin.hori();
            bar_height = size.height + margin.top;
        } else {
            bar_height = 0;
        }

        // 测量剩余子 View
        for (int i = 0; i < count; ++i) {
            auto child = getChildAt(i);
            if (child->getVisibility() != VANISHED) {
                auto new_info = parent_info;
                new_info.setHeight(SizeInfo::Value(parent_info.height.val - bar_height, parent_info.height.mode));
                determineChildSize(child, new_info);

                auto& size = child->getDeterminedSize();
                auto& margin = child->getLayoutMargin();
                int child_width = size.width + margin.hori();
                if (child_width > *wrapped_width) {
                    *wrapped_width = child_width;
                }

                int child_height = size.height + margin.vert();
                if (child_height > *wrapped_height) {
                    *wrapped_height = child_height;
                }
            }
        }

        // 计算最大包围高度
        if (bar) {
            auto& margin = bar->getLayoutMargin();
            if (*wrapped_height < margin.bottom) {
                *wrapped_height = margin.bottom;
            }
            *wrapped_height += bar_height;
        }
    }

}