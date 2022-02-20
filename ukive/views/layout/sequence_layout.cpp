// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "sequence_layout.h"

#include <algorithm>
#include <cmath>
#include <typeinfo>

#include "utils/strings/string_utils.hpp"

#include "ukive/resources/dimension_utils.h"
#include "ukive/views/layout_info/sequence_layout_info.h"

#include "necro/layout_constants.h"


namespace ukive {

    SequenceLayoutInfo::Position resolvePosition(const std::string& val) {
        if (val == necro::kAttrValSeqLayoutPosStart) {
            return SequenceLayoutInfo::Position::START;
        }
        if (val == necro::kAttrValSeqLayoutPosCenter) {
            return SequenceLayoutInfo::Position::CENTER;
        }
        if (val == necro::kAttrValSeqLayoutPosEnd) {
            return SequenceLayoutInfo::Position::END;
        }
        return SequenceLayoutInfo::Position::START;
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

        auto vert_pos = resolveAttrString(attrs, necro::kAttrSeqLayoutVertPos, "start");
        lp->vertical_position = resolvePosition(vert_pos);

        auto hori_pos = resolveAttrString(attrs, necro::kAttrSeqLayoutHoriPos, "start");
        lp->horizontal_position = resolvePosition(hori_pos);

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
                    child->getDeterminedSize().height + vert_margin :
                    child->getDeterminedSize().width + hori_margin;
            }
        }

        for (auto child : delayed_views) {
            auto child_li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());
            auto& child_ls = child->getLayoutSize();

            int hori_margin = child->getLayoutMargin().hori();
            int vert_margin = child->getLayoutMargin().vert();

            auto child_width = SizeInfo::getChildSizeInfo(
                parent_info.width,
                hori_margin + getPadding().hori(),
                child_ls.width);

            auto child_height = SizeInfo::getChildSizeInfo(
                parent_info.height,
                vert_margin + getPadding().vert(),
                child_ls.height);


            if (orientation_ == VERTICAL) {
                int child_h = int(std::round(
                    child_li->weight / static_cast<float>(total_weight)*(parent_info.height.val - cur_size)));
                child_h = (std::max)(0, child_h - vert_margin);
                child_height.val = child_h;
            } else if (orientation_ == HORIZONTAL) {
                int child_w = int(std::round(
                    child_li->weight / static_cast<float>(total_weight)*(parent_info.width.val - cur_size)));
                child_w = (std::max)(0, child_w - hori_margin);
                child_width.val = child_w;
            }


            child->measure(SizeInfo(child_width, child_height));
        }
    }

    void SequenceLayout::determineSeqLayoutChildrenSize(const SizeInfo& parent_info) {
        int total_weight = 0;
        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());
                total_weight += li->weight;
            }
        }

        if (total_weight > 0 &&
            (orientation_ == VERTICAL ?
                parent_info.height.mode != SizeInfo::FREEDOM :
                parent_info.width.mode != SizeInfo::FREEDOM))
        {
            determineWeightedChildrenSize(total_weight, parent_info);
        } else {
            determineChildrenSize(parent_info);
        }
    }

    Size SequenceLayout::getVerticalSize(const SizeInfo& info) {
        int final_width;
        int final_height;

        determineSeqLayoutChildrenSize(info);

        int total_height = 0;

        switch (info.width.mode) {
        case SizeInfo::CONTENT:
            final_width = getWrappedWidth();
            final_width = (std::min)(final_width + getPadding().hori(), info.width.val);
            break;

        case SizeInfo::FREEDOM:
            final_width = getWrappedWidth() + getPadding().hori();
            break;

        case SizeInfo::DEFINED:
        default:
            final_width = info.width.val;
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT: {
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    total_height += child->getDeterminedSize().height + child->getLayoutMargin().vert();
                }
            }
            final_height = (std::min)(info.height.val, total_height + getPadding().vert());
            break;
        }

        case SizeInfo::FREEDOM: {
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    total_height += child->getDeterminedSize().height + child->getLayoutMargin().vert();
                }
            }
            final_height = total_height + getPadding().vert();
            break;
        }

        case SizeInfo::DEFINED:
        default:
            final_height = info.height.val;
            break;
        }

        return Size(final_width, final_height);
    }

    Size SequenceLayout::getHorizontalSize(const SizeInfo& info) {
        int final_width;
        int final_height;

        determineSeqLayoutChildrenSize(info);

        int total_width = 0;

        switch (info.width.mode) {
        case SizeInfo::CONTENT: {
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    total_width += child->getDeterminedSize().width + child->getLayoutMargin().hori();
                }
            }
            final_width = (std::min)(total_width + getPadding().hori(), info.width.val);
            break;
        }

        case SizeInfo::FREEDOM: {
            for (auto child : *this) {
                if (child->getVisibility() != VANISHED) {
                    total_width += child->getDeterminedSize().width + child->getLayoutMargin().hori();
                }
            }
            final_width = total_width + getPadding().hori();
            break;
        }

        case SizeInfo::DEFINED:
        default:
            final_width = info.width.val;
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT:
            final_height = getWrappedHeight();
            final_height = (std::min)(final_height + getPadding().vert(), info.height.val);
            final_height = (std::max)(getDeterminedSize().height, final_height);
            break;

        case SizeInfo::FREEDOM:
            final_height = getWrappedHeight() + getPadding().vert();
            final_height = (std::max)(getDeterminedSize().height, final_height);
            break;

        case SizeInfo::DEFINED:
        default:
            final_height = info.height.val;
            break;
        }

        return Size(final_width, final_height);
    }

    void SequenceLayout::layoutVertical(const Rect& parent_bounds) {
        int cur_top = getPadding().top;
        auto bounds = getContentBounds();

        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());

                auto& size = child->getDeterminedSize();

                cur_top += margin.top;

                int child_left;
                switch (li->horizontal_position) {
                case SequenceLayoutInfo::Position::CENTER:
                    child_left = getPadding().start + margin.start + (bounds.width() - size.width) / 2;
                    break;
                case SequenceLayoutInfo::Position::END:
                    child_left = bounds.right - margin.end - size.width;
                    break;
                default:
                    child_left = getPadding().start + margin.start;
                    break;
                }

                child->layout(
                    Rect(child_left, cur_top, size.width, size.height));

                cur_top += size.height + margin.bottom;
            }
        }
    }

    void SequenceLayout::layoutHorizontal(const Rect& parent_bounds) {
        int cur_left = getPadding().start;
        auto bounds = getContentBounds();

        for (auto child : *this) {
            if (child->getVisibility() != VANISHED) {
                auto& margin = child->getLayoutMargin();
                auto li = static_cast<SequenceLayoutInfo*>(child->getExtraLayoutInfo());

                auto& size = child->getDeterminedSize();

                cur_left += margin.start;

                int child_top;
                switch (li->vertical_position) {
                case SequenceLayoutInfo::Position::CENTER:
                    child_top = getPadding().top + margin.top + (bounds.height() - size.height) / 2;
                    break;
                case SequenceLayoutInfo::Position::END:
                    child_top = bounds.bottom - margin.bottom - size.height;
                    break;
                default:
                    child_top = getPadding().top + margin.top;
                    break;
                }

                child->layout(
                    Rect(cur_left, child_top, size.width, size.height));

                cur_left += size.width + margin.end;
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

}
