// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "sequence_layout.h"

#include <algorithm>
#include <cmath>
#include <typeinfo>

#include "utils/log.h"
#include "utils/strings/string_utils.hpp"

#include "ukive/resources/attr_utils.h"
#include "ukive/views/layout_info/sequence_layout_info.h"

#include "necro/layout_constants.h"


namespace ukive {

    SequenceLayoutInfo::Align resolveAlign(const std::string& val) {
        if (val == necro::kAttrValSeqLayoutPosStart) {
            return SequenceLayoutInfo::Align::START;
        }
        if (val == necro::kAttrValSeqLayoutPosCenter) {
            return SequenceLayoutInfo::Align::CENTER;
        }
        if (val == necro::kAttrValSeqLayoutPosEnd) {
            return SequenceLayoutInfo::Align::END;
        }
        return SequenceLayoutInfo::Align::START;
    }

}

namespace ukive {

    SequenceLayout::SequenceLayout(Context c)
        : SequenceLayout(c, {}) {}

    SequenceLayout::SequenceLayout(Context c, AttrsRef attrs)
        : LayoutView(c, attrs),
          orientation_(VERTICAL)
    {
        auto ori = resolveAttrString(
            attrs, necro::kAttrSeqLayoutViewOri, necro::kAttrValSeqLayoutViewOriVert);
        if (utl::isLitEqual(ori, necro::kAttrValSeqLayoutViewOriVert)) {
            orientation_ = VERTICAL;
        } else if (utl::isLitEqual(ori, necro::kAttrValSeqLayoutViewOriHori)) {
            orientation_ = HORIZONTAL;
        }
    }

    LayoutInfo* SequenceLayout::makeExtraLayoutInfo() const {
        return new SequenceLayoutInfo();
    }

    bool SequenceLayout::isValidExtraLayoutInfo(LayoutInfo* lp) const {
        return typeid(*lp) == typeid(SequenceLayoutInfo);
    }

    LayoutInfo* SequenceLayout::makeExtraLayoutInfo(AttrsRef attrs) const {
        auto lp = new SequenceLayoutInfo();

        auto weight = resolveAttrInt(attrs, necro::kAttrSeqLayoutWeight, 0);
        lp->weight = weight;

        auto vert_align = resolveAttrString(attrs, necro::kAttrSeqLayoutVertAlign, "start");
        lp->vert_align = resolveAlign(vert_align);

        auto hori_align = resolveAttrString(attrs, necro::kAttrSeqLayoutHoriAlign, "start");
        lp->hori_align = resolveAlign(hori_align);

        return lp;
    }

    void SequenceLayout::determineWeightedChildrenSize(
        int total_weight, const SizeInfo& parent_info)
    {
        int cur_size = 0;
        std::vector<View*> delayed_views;
        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto child_li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());
                if (child_li->weight > 0) {
                    delayed_views.push_back(child);
                    continue;
                }

                int hori_margin = child->getLayoutMargin().hori();
                int vert_margin = child->getLayoutMargin().vert();

                determineChildSize(child, parent_info);
                cur_size += (orientation_ == VERTICAL) ?
                    child->getDeterminedSize().height() + vert_margin :
                    child->getDeterminedSize().width() + hori_margin;
            }
        }

        for (auto child : delayed_views) {
            auto child_li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());
            auto& child_ls = child->getLayoutSize();

            int hori_margin = child->getLayoutMargin().hori();
            int vert_margin = child->getLayoutMargin().vert();

            auto child_width = SizeInfo::getChildSizeInfo(
                parent_info.width(),
                hori_margin + getPadding().hori(),
                getMinimumSize().width(),
                child_ls.width());

            auto child_height = SizeInfo::getChildSizeInfo(
                parent_info.height(),
                vert_margin + getPadding().vert(),
                getMinimumSize().height(),
                child_ls.height());


            if (orientation_ == VERTICAL) {
                int child_h = int(std::round(
                    child_li->weight / static_cast<float>(total_weight)*(parent_info.height().val - cur_size)));
                child_h = (std::max)(0, child_h - vert_margin);
                child_height.val = child_h;
            } else if (orientation_ == HORIZONTAL) {
                int child_w = int(std::round(
                    child_li->weight / static_cast<float>(total_weight)*(parent_info.width().val - cur_size)));
                child_w = (std::max)(0, child_w - hori_margin);
                child_width.val = child_w;
            }


            child->determineSize(SizeInfo(child_width, child_height));
        }
    }

    int SequenceLayout::getVerticalFinalHeight(const SizeInfo& info) {
        int final_height;

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
        {
            // 子 View 竖向叠起来。此时 weight 无作用。
            int cur_height = 0;
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    auto w_val = getChildWidthValue(child, info.width(), 0);
                    auto h_val = getChildHeightValue(child, info.height(), cur_height);
                    child->determineSize(SizeInfo(w_val, h_val));

                    int child_h_space = child->getDeterminedSize().height() + child->getLayoutMargin().vert();
                    cur_height += child_h_space;
                }
            }
            final_height = (std::min)(info.height().val, cur_height + getPadding().vert());
            break;
        }

        case SizeInfo::FREEDOM:
        {
            // 子 View 竖向叠起来。此时 weight 无作用。
            int cur_height = 0;
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    auto w_val = getChildWidthValue(child, info.width(), 0);
                    auto h_val = getChildHeightValue(child, info.height(), cur_height);
                    child->determineSize(SizeInfo(w_val, h_val));

                    int child_h_space = child->getDeterminedSize().height() + child->getLayoutMargin().vert();
                    cur_height += child_h_space;
                }
            }
            final_height = cur_height + getPadding().vert();
            break;
        }

        case SizeInfo::DEFINED:
        default:
        {
            /**
             * 子 View 竖向叠起来，同时可以使用 fill 布局和 weight 参数。
             * 应先测量 weight 为 0 而且使用 auto 或 free 布局的子 View。
             * 余下的将由 weight 参数确定其占据的剩余空间的比例。
             */
            int rem_count = 0;
            int cur_height = 0;
            int total_weight = 0;
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    auto& ls = child->getLayoutSize();
                    auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());
                    total_weight += li->weight;

                    // margin 总是计算在内
                    int child_h_space = child->getLayoutMargin().vert();

                    if (li->weight == 0 &&
                        ls.height() != View::LS_FILL)
                    {
                        auto w_val = getChildWidthValue(child, info.width(), 0);
                        auto h_val = getChildHeightValue(child, info.height(), cur_height);
                        child->determineSize(SizeInfo(w_val, h_val));

                        child_h_space += child->getDeterminedSize().height();
                    } else {
                        ++rem_count;
                    }
                    cur_height += child_h_space;
                }
            }

            int rem_height = (std::max)(
                0, info.height().val - getPadding().vert() - cur_height);

            if (total_weight == 0) {
                // 这种情况，布局为 fill 的子 View 平分剩余空间
                int child_h = rem_count ? (rem_height / rem_count) : 0;
                int child_h_rem = rem_count ? (rem_height % rem_count) : 0;
                for (auto child : *this) {
                    if (child->getVisibility() != VANISHED) {
                        auto& ls = child->getLayoutSize();
                        if (ls.height() == View::LS_FILL) {
                            auto w_val = getChildWidthValue(child, info.width(), 0);

                            // 平均分配余数
                            int c_h_val;
                            if (child_h_rem) {
                                c_h_val = child_h + 1;
                                --child_h_rem;
                            } else {
                                c_h_val = child_h;
                            }
                            SizeInfo::Value h_val(child_h, SizeInfo::DEFINED);

                            child->determineSize(SizeInfo(w_val, h_val));
                        }
                    }
                }
            } else {
                // 这种情况，只有 weight 不为 0 的子 View 能获得布局空间
                int alloc_height = 0;
                int cur_count = 0;
                for (auto child : *this) {
                    if (child->getVisibility() != VANISHED) {
                        auto& ls = child->getLayoutSize();
                        auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());

                        if (ls.height() == View::LS_FILL ||
                            li->weight != 0)
                        {
                            ++cur_count;
                            auto w_val = getChildWidthValue(child, info.width(), 0);

                            int child_h = int(std::round(
                                li->weight / static_cast<float>(total_weight) * rem_height));
                            child_h = (std::max)(0, child_h);

                            // 防止超过可用空间
                            if (child_h + alloc_height > rem_height) {
                                child_h = 0;
                            } else {
                                alloc_height += child_h;
                            }

                            // 在该方法之内，子 View 的布局参数不应该发生改变
                            ubassert(cur_count <= rem_count);

                            // 分配余数到最后一个子 View 上面
                            if (cur_count == rem_count &&
                                alloc_height < rem_height)
                            {
                                child_h += rem_height - alloc_height;
                            }
                            SizeInfo::Value h_val(child_h, SizeInfo::DEFINED);

                            child->determineSize(SizeInfo(w_val, h_val));
                        }
                    }
                }
            }
            final_height = info.height().val;
            break;
        }
        }

        return final_height;
    }

    Size SequenceLayout::getVerticalSize(const SizeInfo& info) {
        int final_width;
        int final_height = getVerticalFinalHeight(info);

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
        {
            final_width = getWrappedWidth();
            final_width = (std::min)(final_width + getPadding().hori(), info.width().val);
            final_width = (std::max)(final_width, getMinimumSize().width());

            // 确定了 layout 的宽度之后，需要再看一下那些宽度为 FILL 的子 View
            SizeInfo::Value wv(final_width, SizeInfo::DEFINED);
            SizeInfo::Value hv = info.height();
            final_height = getVerticalFinalHeight(SizeInfo(wv, hv));
            break;
        }

        case SizeInfo::FREEDOM:
            final_width = getWrappedWidth() + getPadding().hori();
            break;

        case SizeInfo::DEFINED:
        default:
            final_width = info.width().val;
            break;
        }

        return Size(final_width, final_height);
    }

    Size SequenceLayout::getHorizontalSize(const SizeInfo& info) {
        int final_width;
        int final_height;

        switch (info.width().mode) {
        case SizeInfo::CONTENT:
        {
            // 子 View 横向叠起来。此时 weight 无作用。
            int cur_width = 0;
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    auto w_val = getChildWidthValue(child, info.width(), cur_width);
                    auto h_val = getChildHeightValue(child, info.height(), 0);
                    child->determineSize(SizeInfo(w_val, h_val));

                    int child_w_space = child->getDeterminedSize().width() + child->getLayoutMargin().hori();
                    cur_width += child_w_space;
                }
            }
            final_width = (std::min)(info.width().val, cur_width + getPadding().hori());
            break;
        }

        case SizeInfo::FREEDOM:
        {
            // 子 View 横向叠起来。此时 weight 无作用。
            int cur_width = 0;
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    auto w_val = getChildWidthValue(child, info.width(), cur_width);
                    auto h_val = getChildHeightValue(child, info.height(), 0);
                    child->determineSize(SizeInfo(w_val, h_val));

                    int child_w_space = child->getDeterminedSize().width() + child->getLayoutMargin().hori();
                    cur_width += child_w_space;
                }
            }
            final_width = cur_width + getPadding().hori();
            break;
        }

        case SizeInfo::DEFINED:
        default:
        {
            /**
             * 子 View 横向叠起来，同时可以使用 fill 布局和 weight 参数。
             * 应先测量 weight 为 0 而且使用 auto 或 free 布局的子 View。
             * 余下的将由 weight 参数确定其占据的剩余空间的比例。
             */
            int rem_count = 0;
            int cur_width = 0;
            int total_weight = 0;
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    auto& ls = child->getLayoutSize();
                    auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());
                    total_weight += li->weight;

                    // margin 总是计算在内
                    int child_w_space = child->getLayoutMargin().hori();

                    if (li->weight == 0 &&
                        ls.width() != View::LS_FILL)
                    {
                        auto w_val = getChildWidthValue(child, info.width(), cur_width);
                        auto h_val = getChildHeightValue(child, info.height(), 0);
                        child->determineSize(SizeInfo(w_val, h_val));

                        child_w_space += child->getDeterminedSize().width();
                    } else {
                        ++rem_count;
                    }
                    cur_width += child_w_space;
                }
            }

            int rem_width = (std::max)(
                0, info.width().val - getPadding().hori() - cur_width);

            if (total_weight == 0) {
                // 这种情况，布局为 fill 的子 View 平分剩余空间
                int child_w = rem_count ? (rem_width / rem_count) : 0;
                int child_w_rem = rem_count ? (rem_width % rem_count) : 0;
                for (auto child : *this) {
                    if (child->getVisibility() != VANISHED) {
                        auto& ls = child->getLayoutSize();
                        if (ls.width() == View::LS_FILL) {
                            auto h_val = getChildHeightValue(child, info.height(), 0);

                            // 平均分配余数
                            int c_w_val;
                            if (child_w_rem) {
                                c_w_val = child_w + 1;
                                --child_w_rem;
                            } else {
                                c_w_val = child_w;
                            }
                            SizeInfo::Value w_val(child_w, SizeInfo::DEFINED);

                            child->determineSize(SizeInfo(w_val, h_val));
                        }
                    }
                }
            } else {
                // 这种情况，只有 weight 不为 0 的子 View 能获得布局空间
                int alloc_width = 0;
                int cur_count = 0;
                for (auto child : *this) {
                    if (child->getVisibility() != VANISHED) {
                        auto& ls = child->getLayoutSize();
                        auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());

                        if (ls.width() == View::LS_FILL ||
                            li->weight != 0)
                        {
                            ++cur_count;
                            auto h_val = getChildHeightValue(child, info.height(), 0);

                            int child_w = int(std::round(
                                li->weight / static_cast<float>(total_weight) * rem_width));
                            child_w = (std::max)(0, child_w);

                            // 防止超过可用空间
                            if (child_w + alloc_width > rem_width) {
                                child_w = 0;
                            } else {
                                alloc_width += child_w;
                            }

                            // 在该方法之内，子 View 的布局参数不应该发生改变
                            ubassert(cur_count <= rem_count);

                            // 分配余数到最后一个子 View 上面
                            if (cur_count == rem_count &&
                                alloc_width < rem_width)
                            {
                                child_w += rem_width - alloc_width;
                            }
                            SizeInfo::Value w_val(child_w, SizeInfo::DEFINED);

                            child->determineSize(SizeInfo(w_val, h_val));
                        }
                    }
                }
            }
            final_width = info.width().val;
            break;
        }
        }

        switch (info.height().mode) {
        case SizeInfo::CONTENT:
            final_height = getWrappedHeight();
            final_height = (std::min)(final_height + getPadding().vert(), info.height().val);
            break;

        case SizeInfo::FREEDOM:
            final_height = getWrappedHeight() + getPadding().vert();
            break;

        case SizeInfo::DEFINED:
        default:
            final_height = info.height().val;
            break;
        }

        return Size(final_width, final_height);
    }

    void SequenceLayout::layoutVertical(const Rect& parent_bounds) {
        int cur_top = getPadding().top();
        auto bounds = getContentBounds();

        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());

                auto size = child->getDeterminedSize();

                cur_top += margin.top();

                int child_left;
                switch (li->hori_align) {
                case SequenceLayoutInfo::Align::CENTER:
                    child_left = getPadding().start() + margin.start()
                        + (bounds.width() - margin.hori() - size.width()) / 2;
                    break;
                case SequenceLayoutInfo::Align::END:
                    child_left = bounds.right() - margin.end() - size.width();
                    break;
                default:
                    child_left = getPadding().start() + margin.start();
                    break;
                }

                child->layout(
                    Rect(child_left, cur_top, size.width(), size.height()));

                cur_top += size.height() + margin.bottom();
            }
        }
    }

    void SequenceLayout::layoutHorizontal(const Rect& parent_bounds) {
        int cur_left = getPadding().start();
        auto bounds = getContentBounds();

        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());

                auto& size = child->getDeterminedSize();

                cur_left += margin.start();

                int child_top;
                switch (li->vert_align) {
                case SequenceLayoutInfo::Align::CENTER:
                    child_top = getPadding().top() + margin.top()
                        + (bounds.height() - margin.vert() - size.height()) / 2;
                    break;
                case SequenceLayoutInfo::Align::END:
                    child_top = bounds.bottom() - margin.bottom() - size.height();
                    break;
                default:
                    child_top = getPadding().top() + margin.top();
                    break;
                }

                child->layout(
                    Rect(cur_left, child_top, size.width(), size.height()));

                cur_left += size.width() + margin.end();
            }
        }
    }

    Size SequenceLayout::onDetermineSize(const SizeInfo& info) {
        if (orientation_ == VERTICAL) {
            return getVerticalSize(info);
        } else {
            return getHorizontalSize(info);
        }
    }

    void SequenceLayout::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        if (orientation_ == VERTICAL) {
            layoutVertical(new_bounds);
        } else {
            layoutHorizontal(new_bounds);
        }
    }

    void SequenceLayout::setOrientation(int orientation) {
        if (orientation == orientation_) {
            return;
        }

        orientation_ = orientation;

        requestLayout();
        requestDraw();
    }

    int SequenceLayout::getOrientation() const {
        return orientation_;
    }

}
