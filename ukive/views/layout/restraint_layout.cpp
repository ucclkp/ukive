// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "restraint_layout.h"

#include <algorithm>
#include <typeinfo>

#include "utils/log.h"
#include "utils/strings/int_conv.hpp"
#include "utils/strings/string_utils.hpp"

#include "ukive/resources/dimension_utils.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/window/window.h"

#include "necro/layout_constants.h"


namespace ukive {

    bool resolveEdge(const std::string& sh_edge_str, RestraintLayoutInfo::Edge* edge) {
        auto edge_str = sh_edge_str;
        utl::tolatl(&edge_str);
        if (edge_str == necro::kAttrValRestraintLayoutHEStart) {
            *edge = RestraintLayoutInfo::Edge::START;
        } else if (edge_str == necro::kAttrValRestraintLayoutHEEnd) {
            *edge = RestraintLayoutInfo::Edge::END;
        } else if (edge_str == necro::kAttrValRestraintLayoutHETop) {
            *edge = RestraintLayoutInfo::Edge::TOP;
        } else if (edge_str == necro::kAttrValRestraintLayoutHEBottom) {
            *edge = RestraintLayoutInfo::Edge::BOTTOM;
        } else {
            return false;
        }
        return true;
    }

}

namespace ukive {

    RestraintLayout::RestraintLayout(Context c)
        : RestraintLayout(c, {}) {}

    RestraintLayout::RestraintLayout(Context c, AttrsRef attrs)
        : LayoutView(c, attrs) {}

    LayoutInfo* RestraintLayout::makeExtraLayoutInfo() const {
        return new RestraintLayoutInfo();
    }

    LayoutInfo* RestraintLayout::makeExtraLayoutInfo(AttrsRef attrs) const {
        auto lp = new RestraintLayoutInfo();

        // Start Handle
        auto start_handle = resolveAttrString(attrs, necro::kAttrRestraintLayoutSH, {});
        if (!start_handle.empty()) {
            auto params = utl::split(start_handle, ",");
            if (params.size() > 0) {
                int id = -1;
                auto param = params[0];
                utl::trim(&param);
                utl::stoi(param, &id);
                if (id != -1) {
                    auto edge = RestraintLayoutInfo::Edge::START;
                    if (params.size() > 1) {
                        param = params[1];
                        utl::trim(&param);
                        resolveEdge(param, &edge);
                    }
                    lp->startHandle(id, edge);
                }
            }
        }

        // End Handle
        auto end_handle = resolveAttrString(attrs, necro::kAttrRestraintLayoutEH, {});
        if (!end_handle.empty()) {
            auto params = utl::split(end_handle, ",");
            if (params.size() > 0) {
                int id = -1;
                auto param = params[0];
                utl::trim(&param);
                utl::stoi(param, &id);
                if (id != -1) {
                    auto edge = RestraintLayoutInfo::Edge::END;
                    if (params.size() > 1) {
                        param = params[1];
                        utl::trim(&param);
                        resolveEdge(param, &edge);
                    }
                    lp->endHandle(id, edge);
                }
            }
        }

        // Top Handle
        auto top_handle = resolveAttrString(attrs, necro::kAttrRestraintLayoutTH, {});
        if (!top_handle.empty()) {
            auto params = utl::split(top_handle, ",");
            if (params.size() > 0) {
                int id = -1;
                auto param = params[0];
                utl::trim(&param);
                utl::stoi(param, &id);
                if (id != -1) {
                    auto edge = RestraintLayoutInfo::Edge::TOP;
                    if (params.size() > 1) {
                        param = params[1];
                        utl::trim(&param);
                        resolveEdge(param, &edge);
                    }
                    lp->topHandle(id, edge);
                }
            }
        }

        // Bottom Handle
        auto bottom_handle = resolveAttrString(attrs, necro::kAttrRestraintLayoutBH, {});
        if (!bottom_handle.empty()) {
            auto params = utl::split(bottom_handle, ",");
            if (params.size() > 0) {
                int id = -1;
                auto param = params[0];
                utl::trim(&param);
                utl::stoi(param, &id);
                if (id != -1) {
                    auto edge = RestraintLayoutInfo::Edge::BOTTOM;
                    if (params.size() > 1) {
                        param = params[1];
                        utl::trim(&param);
                        resolveEdge(param, &edge);
                    }
                    lp->bottomHandle(id, edge);
                }
            }
        }

        return lp;
    }

    bool RestraintLayout::isValidExtraLayoutInfo(LayoutInfo* lp) const {
        return typeid(*lp) == typeid(RestraintLayoutInfo);
    }

    void RestraintLayout::clearMeasureFlag() {
        for (auto child : *this) {
            auto li = static_cast<RestraintLayoutInfo*>(child->getExtraLayoutInfo());
            li->is_width_determined = li->is_height_determined = false;
            li->is_vert_layouted = li->is_hori_layouted = false;
        }
    }

    bool RestraintLayout::isAttended(View* v) const {
        return v->getVisibility() != VANISHED;
    }

    void RestraintLayout::measureRestrainedChildren(const SizeInfo& parent_info) {
        for (auto child : *this) {
            auto li = static_cast<RestraintLayoutInfo*>(child->getExtraLayoutInfo());
            if (li->is_width_determined) {
                continue;
            }

            SizeInfo::Value child_width;
            getRestrainedChildWidth(
                child, li,
                parent_info.width,
                &child_width);

            li->is_width_determined = true;
            li->width_info = child_width;
        }

        for (auto child : *this) {
            auto li = static_cast<RestraintLayoutInfo*>(child->getExtraLayoutInfo());
            if (li->is_width_determined && li->is_height_determined) {
                continue;
            }

            SizeInfo::Value child_height;
            if (!li->is_height_determined) {
                getRestrainedChildHeight(
                    child, li,
                    parent_info.height,
                    &child_height);

                li->is_height_determined = true;
                li->height_info = child_height;
            }
            else {
                child_height = li->height_info;
            }

            if (isAttended(child)) {
                child->measure(SizeInfo(li->width_info, child_height));
            }
        }
    }

    void RestraintLayout::getRestrainedChildWidth(
        View* child, RestraintLayoutInfo* rli,
        const SizeInfo::Value& parent_width, SizeInfo::Value* width)
    {
        auto& margin = child->getLayoutMargin();
        auto& layout_size = child->getLayoutSize();

        SizeInfo::Value child_width;
        int hori_margins = margin.hori();

        if (rli->hasHoriCouple()) {
            int size = std::max(0,
                parent_width.val - getPadding().hori() - hori_margins);

            // child 有固定的大小。
            if (layout_size.width >= 0 || !isAttended(child)) {
                child_width = SizeInfo::Value(
                    isAttended(child) ? layout_size.width : 0, SizeInfo::DEFINED);
                rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FIXED;
            }
            // child 将填充 handler couple 之间的区域。
            else {
                // handler couple 绑定于父 View。
                if (rli->start_handle_id == getId()
                    && rli->end_handle_id == getId())
                {
                    switch (parent_width.mode) {
                    case SizeInfo::CONTENT:
                    {
                        if (layout_size.width == LS_FILL) {
                            child_width = SizeInfo::Value(size, SizeInfo::CONTENT);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        } else if (layout_size.width == LS_AUTO) {
                            child_width = SizeInfo::Value(size, SizeInfo::CONTENT);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        } else {
                            child_width = SizeInfo::Value(size, SizeInfo::FREEDOM);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        break;
                    }
                    case SizeInfo::DEFINED:
                    {
                        if (layout_size.width == LS_FILL) {
                            child_width = SizeInfo::Value(size, SizeInfo::DEFINED);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        } else if (layout_size.width == LS_AUTO) {
                            child_width = SizeInfo::Value(size, SizeInfo::CONTENT);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        } else {
                            child_width = SizeInfo::Value(size, SizeInfo::FREEDOM);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        break;
                    }
                    case SizeInfo::FREEDOM:
                    default:
                    {
                        // 此处，由于不知道父 View 的大小，无法填充 handler couple 之间的区域。
                        // 让 View 自己决定大小即可。
                        child_width = SizeInfo::Value(size, SizeInfo::FREEDOM);
                        rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        break;
                    }
                    }
                }
                // handler couple 没有绑定于父 View。这意味着它与其他子 View 绑定。
                // 此时必须先测量绑定到的 View，然后才能测量该 View。将使用
                // 递归完成此过程，为此需要额外的变量来记录某一 View 是否已测量。
                else
                {
                    // 左右 Handler 绑定在同一个 View 上。
                    if (rli->start_handle_id == rli->end_handle_id)
                    {
                        int measured_target_width = 0;
                        View* target = getChildById(rli->start_handle_id);
                        auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

                        // 测量此 target view 的宽度。
                        SizeInfo::Value target_width;
                        if (!target_li->is_width_determined)
                        {
                            getRestrainedChildWidth(target, target_li, parent_width, &target_width);

                            target_li->is_width_determined = true;
                            target_li->width_info = target_width;
                        }
                        else
                        {
                            target_width = target_li->width_info;
                        }

                        SizeInfo::Value target_height(0, SizeInfo::FREEDOM);
                        if (target_li->is_height_determined)
                        {
                            target_height = target_li->height_info;
                        }

                        if (isAttended(target)) {
                            // 让 target view 测量自身。
                            // 这将会使 target view 的 onDetermineSize() 方法多调用一次。
                            target->measure(SizeInfo(target_width, target_height));
                            measured_target_width = target->getDeterminedSize().width;
                        }

                        child_width.val = std::max(0, measured_target_width - hori_margins);

                        if (layout_size.width == LS_FILL)
                        {
                            child_width.mode = SizeInfo::DEFINED;
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        }
                        else if (layout_size.width == LS_AUTO)
                        {
                            child_width.mode = SizeInfo::CONTENT;
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        else
                        {
                            child_width.mode = SizeInfo::FREEDOM;
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                    }
                    else
                    {
                        if (parent_width.mode == SizeInfo::DEFINED
                            || parent_width.mode == SizeInfo::CONTENT)
                        {
                            // 前向遍历。
                            int measured_start_margin = margin.start;
                            RestraintLayoutInfo* child_li = rli;
                            while (child_li->hasStart()
                                && child_li->start_handle_id != this->getId())
                            {
                                View* target = getChildById(child_li->start_handle_id);
                                auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

                                // 测量此 target view 的宽度。
                                SizeInfo::Value target_width;
                                if (!target_li->is_width_determined)
                                {
                                    getRestrainedChildWidth(
                                        target, target_li, parent_width, &target_width);

                                    target_li->is_width_determined = true;
                                    target_li->width_info = target_width;
                                }
                                else
                                {
                                    target_width = target_li->width_info;
                                }

                                SizeInfo::Value target_height(0, SizeInfo::FREEDOM);
                                if (target_li->is_height_determined)
                                {
                                    target_height = target_li->height_info;
                                }

                                if (isAttended(target)) {
                                    // 让 target view 测量自身。
                                    // 这将会使 target view 的 onDetermineSize() 方法多调用一次。
                                    target->measure(SizeInfo(target_width, target_height));
                                    auto& target_margin = target->getLayoutMargin();

                                    if (child_li->start_handle_edge == RestraintLayoutInfo::END)
                                    {
                                        measured_start_margin += target->getDeterminedSize().width
                                            + (target_li->hasStart() ? target_margin.start : 0);
                                    }
                                    else if (child_li->start_handle_edge == RestraintLayoutInfo::START)
                                    {
                                        measured_start_margin +=
                                            (target_li->hasStart() ? target_margin.start : 0);
                                    }
                                }

                                child_li = target_li;
                            }

                            // 后向遍历。
                            int measured_end_margin = margin.end;
                            child_li = rli;
                            while (child_li->hasEnd()
                                && child_li->end_handle_id != this->getId())
                            {
                                View* target = getChildById(child_li->end_handle_id);
                                auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

                                // 测量此 target view 的宽度。
                                SizeInfo::Value target_width;
                                if (!target_li->is_width_determined)
                                {
                                    getRestrainedChildWidth(
                                        target, target_li, parent_width, &target_width);

                                    target_li->is_width_determined = true;
                                    target_li->width_info = target_width;
                                }
                                else
                                {
                                    target_width = target_li->width_info;
                                }

                                SizeInfo::Value target_height(0, SizeInfo::FREEDOM);
                                if (target_li->is_height_determined)
                                {
                                    target_height = target_li->height_info;
                                }

                                if (isAttended(target)) {
                                    // 让 target view 测量自身。
                                    // 这将会使 target view 的 onDetermineSize() 方法多调用一次。
                                    target->measure(SizeInfo(target_width, target_height));
                                    auto& target_margin = target->getLayoutMargin();

                                    if (child_li->end_handle_edge
                                        == RestraintLayoutInfo::START)
                                    {
                                        measured_end_margin += target->getDeterminedSize().width
                                            + (target_li->hasEnd() ? target_margin.end : 0);
                                    } else if (child_li->end_handle_edge
                                        == RestraintLayoutInfo::END)
                                    {
                                        measured_end_margin +=
                                            (target_li->hasEnd() ? target_margin.end : 0);
                                    }
                                }

                                child_li = target_li;
                            }

                            child_width.val = std::max(0, parent_width.val - getPadding().hori()
                                - measured_start_margin - measured_end_margin);

                            if (layout_size.width == LS_FILL)
                            {
                                if (parent_width.mode == SizeInfo::CONTENT)
                                {
                                    child_width.mode = SizeInfo::CONTENT;
                                    rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                                }
                                else if (parent_width.mode == SizeInfo::DEFINED)
                                {
                                    child_width.mode = SizeInfo::DEFINED;
                                    rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                                }
                            }
                            else if (layout_size.width == LS_AUTO)
                            {
                                child_width.mode = SizeInfo::CONTENT;
                                rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                            }
                            else
                            {
                                child_width.mode = SizeInfo::FREEDOM;
                                rli->hori_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                            }
                        }
                        else if (parent_width.mode == SizeInfo::FREEDOM)
                        {
                            child_width = SizeInfo::Value(size, SizeInfo::FREEDOM);
                            rli->hori_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        }
                    }
                }
            }
        } else {
            if (isAttended(child)) {
                auto new_parent_width = parent_width;
                new_parent_width.mode = SizeInfo::FREEDOM;
                child_width = SizeInfo::getChildSizeInfo(
                    new_parent_width,
                    hori_margins + getPadding().hori(),
                    layout_size.width);
            } else {
                child_width = SizeInfo::Value(0, SizeInfo::DEFINED);
            }
        }

        *width = child_width;
    }

    void RestraintLayout::getRestrainedChildHeight(
        View* child, RestraintLayoutInfo* rli,
        const SizeInfo::Value& parent_height, SizeInfo::Value* height)
    {
        auto& margin = child->getLayoutMargin();
        auto& layout_size = child->getLayoutSize();

        SizeInfo::Value child_height;
        int vert_margins = margin.vert();

        if (rli->hasVertCouple()) {
            int size = std::max(0, parent_height.val - getPadding().vert() - vert_margins);

            // child 有固定的大小。
            if (layout_size.height >= 0 || !isAttended(child)) {
                child_height.val = isAttended(child) ? layout_size.height : 0;
                child_height.mode = SizeInfo::DEFINED;
                rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FIXED;
            }
            // child 将填充 handler couple 之间的区域。
            else {
                // handler couple 绑定于父 View。
                if (rli->top_handle_id == getId() && rli->bottom_handle_id == getId())
                {
                    switch (parent_height.mode) {
                    case SizeInfo::CONTENT:
                    {
                        if (layout_size.height == LS_FILL)
                        {
                            child_height.val = size;
                            child_height.mode = SizeInfo::CONTENT;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        }
                        else if (layout_size.height == LS_AUTO)
                        {
                            child_height.val = size;
                            child_height.mode = SizeInfo::CONTENT;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        else
                        {
                            child_height.val = size;
                            child_height.mode = SizeInfo::FREEDOM;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        break;
                    }
                    case SizeInfo::DEFINED:
                    {
                        if (layout_size.height == LS_FILL)
                        {
                            child_height.val = size;
                            child_height.mode = SizeInfo::DEFINED;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        }
                        else if (layout_size.height == LS_AUTO)
                        {
                            child_height.val = size;
                            child_height.mode = SizeInfo::CONTENT;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        else
                        {
                            child_height.val = size;
                            child_height.mode = SizeInfo::FREEDOM;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        break;
                    }
                    case SizeInfo::FREEDOM:
                    default:
                    {
                        // 此处，由于不知道父 View 的大小，无法填充 handler couple 之间的区域。
                        // 让 View 自己决定大小即可。
                        child_height.val = size;
                        child_height.mode = SizeInfo::FREEDOM;
                        rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        break;
                    }
                    }
                }
                // handler couple 没有绑定于父 View。这意味着它与其他子 View 绑定。
                // 此时必须先测量绑定到的 View，然后才能测量该 View。将使用
                // 递归完成此过程，为此需要额外的变量来记录某一 View 是否已测量。
                else {
                    // 上下 Handler 绑定在同一个 View 上。
                    if (rli->top_handle_id == rli->bottom_handle_id)
                    {
                        int measured_target_height = 0;
                        View* target = getChildById(rli->top_handle_id);
                        auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

                        // 测量此 target view 的高度。
                        SizeInfo::Value targetHeight;
                        if (!target_li->is_height_determined)
                        {
                            getRestrainedChildHeight(
                                target, target_li, parent_height, &targetHeight);

                            target_li->is_height_determined = true;
                            target_li->height_info = targetHeight;
                        }
                        else
                        {
                            targetHeight = target_li->height_info;
                        }

                        SizeInfo::Value target_width(0, SizeInfo::FREEDOM);
                        if (target_li->is_width_determined)
                        {
                            target_width = target_li->width_info;
                        }

                        if (isAttended(target)) {
                            // 让 target view 测量自身。
                            // 这将会使 target view 的 onDetermineSize() 方法多调用一次。
                            target->measure(SizeInfo(target_width, targetHeight));
                            measured_target_height = target->getDeterminedSize().height;
                        }

                        child_height.val = std::max(0, measured_target_height - vert_margins);

                        if (layout_size.height == LS_FILL)
                        {
                            child_height.mode = SizeInfo::DEFINED;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        }
                        else if (layout_size.height == LS_AUTO)
                        {
                            child_height.mode = SizeInfo::CONTENT;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                        else
                        {
                            child_height.mode = SizeInfo::FREEDOM;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                        }
                    } else {
                        if (parent_height.mode == SizeInfo::DEFINED ||
                            parent_height.mode == SizeInfo::CONTENT)
                        {
                            // 上向遍历。
                            int measured_top_margin = margin.top;
                            auto child_li = rli;
                            while (child_li->hasTop()
                                && child_li->top_handle_id != this->getId())
                            {
                                View* target = getChildById(child_li->top_handle_id);
                                auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

                                // 测量此 target view 的高度。
                                SizeInfo::Value target_height;
                                if (!target_li->is_height_determined)
                                {
                                    getRestrainedChildHeight(
                                        target, target_li, parent_height, &target_height);

                                    target_li->is_height_determined = true;
                                    target_li->height_info = child_height;
                                }
                                else
                                {
                                    target_height = target_li->height_info;
                                }

                                SizeInfo::Value target_width(0, SizeInfo::FREEDOM);
                                if (target_li->is_width_determined)
                                {
                                    target_width = target_li->width_info;
                                }

                                if (isAttended(target)) {
                                    // 让 target view 测量自身。
                                    // 这将会使 target view 的 onDetermineSize() 方法多调用一次。
                                    target->measure(SizeInfo(target_width, target_height));
                                    auto& target_margin = target->getLayoutMargin();

                                    if (child_li->top_handle_edge == RestraintLayoutInfo::BOTTOM) {
                                        measured_top_margin += target->getDeterminedSize().height
                                            + (target_li->hasTop() ? target_margin.top : 0);
                                    } else if (child_li->top_handle_edge == RestraintLayoutInfo::TOP) {
                                        measured_top_margin +=
                                            (target_li->hasTop() ? target_margin.top : 0);
                                    }
                                }

                                child_li = target_li;
                            }

                            // 下向遍历。
                            int measured_bottom_margin = margin.bottom;
                            child_li = rli;
                            while (child_li->hasBottom() && child_li->bottom_handle_id != getId())
                            {
                                View* target = getChildById(child_li->bottom_handle_id);
                                auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

                                // 测量此 target view 的高度。
                                SizeInfo::Value target_height;
                                if (!target_li->is_height_determined)
                                {
                                    getRestrainedChildHeight(
                                        target, target_li, parent_height, &target_height);

                                    target_li->is_height_determined = true;
                                    target_li->height_info = target_height;
                                }
                                else
                                {
                                    target_height = target_li->height_info;
                                }

                                SizeInfo::Value target_width(0, SizeInfo::FREEDOM);
                                if (target_li->is_width_determined)
                                {
                                    target_width = target_li->width_info;
                                }

                                if (isAttended(target)) {
                                    // 让 target view 测量自身。
                                    // 这将会使 target view 的 onDetermineSize() 方法多调用一次。
                                    target->measure(SizeInfo(target_width, target_height));
                                    auto& target_margin = target->getLayoutMargin();

                                    if (child_li->bottom_handle_edge == RestraintLayoutInfo::TOP) {
                                        measured_bottom_margin += target->getDeterminedSize().height
                                            + (target_li->hasBottom() ? target_margin.bottom : 0);
                                    } else if (child_li->bottom_handle_edge == RestraintLayoutInfo::BOTTOM) {
                                        measured_bottom_margin +=
                                            (target_li->hasBottom() ? target_margin.bottom : 0);
                                    }
                                }

                                child_li = target_li;
                            }

                            // TODO: 这里不应该用 parent_height，而应该使用最后遍历到的 view
                            // 作为边界来计算。此处假定最终绑定到 parent 边界。
                            child_height.val = std::max(0, parent_height.val - getPadding().vert()
                                - measured_top_margin - measured_bottom_margin);

                            if (layout_size.height == LS_FILL) {
                                if (parent_height.mode == SizeInfo::CONTENT) {
                                    child_height.mode = SizeInfo::CONTENT;
                                    rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                                } else if (parent_height.mode == SizeInfo::DEFINED) {
                                    child_height.mode = SizeInfo::DEFINED;
                                    rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                                }
                            } else if (layout_size.height == LS_AUTO) {
                                child_height.mode = SizeInfo::CONTENT;
                                rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                            } else {
                                child_height.mode = SizeInfo::FREEDOM;
                                rli->vert_couple_handler_type = RestraintLayoutInfo::CH_WRAP;
                            }
                        } else if (parent_height.mode == SizeInfo::FREEDOM) {
                            child_height.val = size;
                            child_height.mode = SizeInfo::FREEDOM;
                            rli->vert_couple_handler_type = RestraintLayoutInfo::CH_FILL;
                        }
                    }
                }
            }
        } else {
            if (isAttended(child)) {
                auto new_parent_height = parent_height;
                new_parent_height.mode = SizeInfo::FREEDOM;
                child_height = SizeInfo::getChildSizeInfo(
                    new_parent_height, vert_margins + getPadding().vert(), layout_size.height);
            } else {
                child_height.val = 0;
                child_height.mode = SizeInfo::DEFINED;
            }
        }

        *height = child_height;
    }

    int RestraintLayout::getLeftSpacing(View* child, RestraintLayoutInfo* rli) {
        int left_spacing = 0;
        auto child_li = rli;
        while (child_li->hasStart() && child_li->start_handle_id != getId()) {
            View* target = getChildById(child_li->start_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());
            auto& margin = target->getLayoutMargin();

            if (isAttended(target)) {
                if (child_li->start_handle_edge == RestraintLayoutInfo::END) {
                    left_spacing += target->getDeterminedSize().width
                        + (target_li->hasStart() ? margin.start : 0);
                } else if (child_li->start_handle_edge == RestraintLayoutInfo::START) {
                    left_spacing +=
                        (target_li->hasStart() ? margin.start : 0);
                }
            }

            child_li = target_li;
        }

        return left_spacing;
    }

    int RestraintLayout::getTopSpacing(View* child, RestraintLayoutInfo* rli) {
        int top_spacing = 0;
        auto child_li = rli;
        while (child_li->hasTop() && child_li->top_handle_id != getId()) {
            View* target = getChildById(child_li->top_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());
            auto& margin = target->getLayoutMargin();

            if (isAttended(target)) {
                if (child_li->top_handle_edge == RestraintLayoutInfo::BOTTOM) {
                    top_spacing += target->getDeterminedSize().height
                        + (target_li->hasTop() ? margin.top : 0);
                } else if (child_li->top_handle_edge == RestraintLayoutInfo::TOP) {
                    top_spacing +=
                        (target_li->hasTop() ? margin.top : 0);
                }
            }

            child_li = target_li;
        }

        return top_spacing;
    }

    int RestraintLayout::getRightSpacing(View* child, RestraintLayoutInfo* rli) {
        int right_spacing = 0;
        auto child_li = rli;
        while (child_li->hasEnd() && child_li->end_handle_id != getId()) {
            View* target = getChildById(child_li->end_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());
            auto& margin = target->getLayoutMargin();

            if (isAttended(target)) {
                if (child_li->end_handle_edge == RestraintLayoutInfo::START) {
                    right_spacing += target->getDeterminedSize().width
                        + (target_li->hasEnd() ? margin.end : 0);
                } else if (child_li->end_handle_edge == RestraintLayoutInfo::END) {
                    right_spacing +=
                        (target_li->hasEnd() ? margin.end : 0);
                }
            }

            child_li = target_li;
        }

        return right_spacing;
    }

    int RestraintLayout::getBottomSpacing(View* child, RestraintLayoutInfo* rli) {
        int bottom_spacing = 0;
        auto child_li = rli;
        while (child_li->hasBottom() && child_li->bottom_handle_id != getId()) {
            auto target = getChildById(child_li->bottom_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());
            auto& margin = target->getLayoutMargin();

            if (isAttended(target)) {
                if (child_li->bottom_handle_edge == RestraintLayoutInfo::TOP) {
                    bottom_spacing += target->getDeterminedSize().height
                        + (target_li->hasBottom() ? margin.bottom : 0);
                } else if (child_li->bottom_handle_edge == RestraintLayoutInfo::BOTTOM) {
                    bottom_spacing +=
                        (target_li->hasBottom() ? margin.bottom : 0);
                }
            }

            child_li = target_li;
        }

        return bottom_spacing;
    }

    int RestraintLayout::measureWrappedWidth() {
        int wrapped_width = 0;

        for (auto child : *this) {
            auto& margin = child->getLayoutMargin();
            auto li = static_cast<RestraintLayoutInfo*>(child->getExtraLayoutInfo());

            int start_spacing = getLeftSpacing(child, li);
            int end_spacing = getRightSpacing(child, li);

            int chain_width = start_spacing + end_spacing;
            if (isAttended(child)) {
                chain_width += child->getDeterminedSize().width
                    + (li->hasStart() ? margin.start : 0)
                    + (li->hasEnd() ? margin.end : 0);
            }

            wrapped_width = std::max(wrapped_width, chain_width);
        }

        return wrapped_width;
    }

    int RestraintLayout::measureWrappedHeight() {
        int wrapped_height = 0;

        for (auto child : *this) {
            auto& margin = child->getLayoutMargin();
            auto li = static_cast<RestraintLayoutInfo*>(child->getExtraLayoutInfo());

            int top_spacing = getTopSpacing(child, li);
            int bottom_spacing = getBottomSpacing(child, li);

            int chain_height = top_spacing + bottom_spacing;
            if (isAttended(child)) {
                chain_height += child->getDeterminedSize().height
                    + (li->hasTop() ? margin.top : 0)
                    + (li->hasBottom() ? margin.bottom : 0);
            }
            wrapped_height = std::max(wrapped_height, chain_height);
        }

        return wrapped_height;
    }

    void RestraintLayout::layoutChild(
        View* child, RestraintLayoutInfo* rli,
        int left, int top, int right, int bottom)
    {
        if (!rli->is_vert_layouted) {
            layoutChildVertical(child, rli, top, bottom);
        }
        if (!rli->is_hori_layouted) {
            layoutChildHorizontal(child, rli, left, right);
        }

        Rect bounds;
        bounds.left = rli->left;
        bounds.top = rli->top;
        bounds.right = rli->right;
        bounds.bottom = rli->bottom;
        child->layout(bounds);
    }

    void RestraintLayout::layoutChildVertical(
        View* child, RestraintLayoutInfo* rli, int top, int bottom)
    {
        auto& margin = child->getLayoutMargin();
        int child_top = top + getPadding().top;
        if (rli->hasTop() && rli->top_handle_id != getId()) {
            auto target = getChildById(rli->top_handle_id);
            auto target_li = reinterpret_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

            if (!target_li->is_vert_layouted) {
                layoutChildVertical(target, target_li, top, bottom);
            }

            if (rli->top_handle_edge == RestraintLayoutInfo::BOTTOM) {
                child_top = target_li->bottom;
            } else if (rli->top_handle_edge == RestraintLayoutInfo::TOP) {
                child_top = target_li->top;
            }
        }

        int child_bottom = bottom - getPadding().bottom;
        if (rli->hasBottom() && rli->bottom_handle_id != getId()) {
            auto target = getChildById(rli->bottom_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

            if (!target_li->is_vert_layouted) {
                layoutChildVertical(target, target_li, top, bottom);
            }

            if (rli->bottom_handle_edge == RestraintLayoutInfo::TOP) {
                child_bottom = target_li->top;
            } else if (rli->bottom_handle_edge == RestraintLayoutInfo::BOTTOM) {
                child_bottom = target_li->bottom;
            }
        }

        if (rli->hasVertCouple()) {
            if (isAttended(child)) {
                child_top += margin.top;
                child_bottom -= margin.bottom;
            }

            switch (rli->vert_couple_handler_type) {
            case RestraintLayoutInfo::CH_FILL:
                break;
            case RestraintLayoutInfo::CH_WRAP:
            case RestraintLayoutInfo::CH_FIXED:
                if (isAttended(child)) {
                    child_top += int(
                        (child_bottom - child_top - child->getDeterminedSize().height)*rli->vert_percent);
                    child_bottom = child_top + child->getDeterminedSize().height;
                } else {
                    child_top += int((child_bottom - child_top)*rli->vert_percent);
                    child_bottom = child_top;
                }
                break;
            }
        } else {
            if (isAttended(child)) {
                if (rli->hasTop()) {
                    child_top += margin.top;
                    child_bottom = child_top + child->getDeterminedSize().height;
                } else if (rli->hasBottom()) {
                    child_bottom -= margin.bottom;
                    child_top = child_bottom - child->getDeterminedSize().height;
                } else
                    child_bottom = child_top + child->getDeterminedSize().height;
            } else {
                if (rli->hasTop()) {
                    child_bottom = child_top;
                } else if (rli->hasBottom()) {
                    child_top = child_bottom;
                } else
                    child_bottom = child_top;
            }
        }

        rli->is_vert_layouted = true;
        rli->top = child_top;
        rli->bottom = child_bottom;
    }

    void RestraintLayout::layoutChildHorizontal(
        View* child, RestraintLayoutInfo* rli,
        int left, int right)
    {
        auto& margin = child->getLayoutMargin();

        int child_left = left + getPadding().start;
        if (rli->hasStart() && rli->start_handle_id != getId()) {
            View* target = getChildById(rli->start_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

            if (!target_li->is_hori_layouted) {
                layoutChildHorizontal(target, target_li, left, right);
            }

            if (rli->start_handle_edge == RestraintLayoutInfo::END) {
                child_left = target_li->right;
            } else if (rli->start_handle_edge == RestraintLayoutInfo::START) {
                child_left = target_li->left;
            }
        }

        int child_right = right - getPadding().end;
        if (rli->hasEnd() && rli->end_handle_id != getId()) {
            View* target = getChildById(rli->end_handle_id);
            auto target_li = static_cast<RestraintLayoutInfo*>(target->getExtraLayoutInfo());

            if (!target_li->is_hori_layouted) {
                layoutChildHorizontal(target, target_li, left, right);
            }

            if (rli->end_handle_edge == RestraintLayoutInfo::START) {
                child_right = target_li->left;
            } else if (rli->end_handle_edge == RestraintLayoutInfo::END) {
                child_right = target_li->right;
            }
        }

        if (rli->hasHoriCouple()) {
            if (isAttended(child)) {
                child_left += margin.start;
                child_right -= margin.end;
            }

            switch (rli->hori_couple_handler_type) {
            case RestraintLayoutInfo::CH_FILL:
                break;
            case RestraintLayoutInfo::CH_WRAP:
            case RestraintLayoutInfo::CH_FIXED:
                if (isAttended(child)) {
                    child_left += int(
                        (child_right - child_left - child->getDeterminedSize().width)*rli->hori_percent);
                    child_right = child_left + child->getDeterminedSize().width;
                } else {
                    child_left += int((child_right - child_left)*rli->hori_percent);
                    child_right = child_left;
                }
                break;
            }
        } else {
            if (isAttended(child)) {
                if (rli->hasStart()) {
                    child_left += margin.start;
                    child_right = child_left + child->getDeterminedSize().width;
                } else if (rli->hasEnd()) {
                    child_right -= margin.end;
                    child_left = child_right - child->getDeterminedSize().width;
                } else {
                    child_right = child_left + child->getDeterminedSize().width;
                }
            } else {
                if (rli->hasStart()) {
                    child_right = child_left;
                } else if (rli->hasEnd()) {
                    child_left = child_right;
                } else {
                    child_right = child_left;
                }
            }
        }

        rli->is_hori_layouted = true;
        rli->left = child_left;
        rli->right = child_right;
    }

    Size RestraintLayout::onDetermineSize(const SizeInfo& info) {
        int final_width = 0;
        int final_height = 0;

        clearMeasureFlag();
        measureRestrainedChildren(info);

        switch (info.width.mode) {
        case SizeInfo::CONTENT:
            final_width = measureWrappedWidth() + getPadding().hori();
            final_width = std::min(info.width.val, final_width);
            break;

        case SizeInfo::DEFINED:
            final_width = info.width.val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_width = measureWrappedWidth() + getPadding().hori();
            break;
        }

        switch (info.height.mode) {
        case SizeInfo::CONTENT:
            final_height = measureWrappedHeight() + getPadding().vert();
            final_height = std::min(info.height.val, final_height);
            break;

        case SizeInfo::DEFINED:
            final_height = info.height.val;
            break;

        case SizeInfo::FREEDOM:
        default:
            final_height = measureWrappedHeight() + getPadding().vert();
            break;
        }

        return Size(final_width, final_height);
    }

    void RestraintLayout::onLayout(
        const Rect& new_bounds, const Rect& old_bounds)
    {
        for (auto child : *this) {
            auto li = static_cast<RestraintLayoutInfo*>(child->getExtraLayoutInfo());
            layoutChild(child, li, 0, 0, new_bounds.width(), new_bounds.height());
        }
    }

}
