// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/list/linear_list_layouter.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item.h"


namespace ukive {

    LinearListLayouter::LinearListLayouter()
        : cur_position_(0),
          cur_offset_in_position_(0) {
    }

    void LinearListLayouter::onMeasureAtPosition(bool cur, int width, int height) {
        if (!isAvailable()) {
            return;
        }

        parent_->freezeLayout();

        int index = 0;
        int total_height = 0;
        int item_count = source_->onListGetDataCount();

        int pos = cur ? cur_position_ : 0;
        int offset = cur ? cur_offset_in_position_ : 0;

        for (int i = pos; i < item_count; ++i, ++index) {
            if (total_height >= height + offset) {
                break;
            }

            auto item = column_.findAndInsertItem(index, source_->onListGetItemId(i));
            if (!item) {
                item = parent_->makeNewItem(i, index);
                column_.addItem(item, index);
            } else {
                item->data_pos = i;
                source_->onListSetItemData(item, i);
            }

            int c_width, c_height;
            parent_->measureItem(item, width, &c_width, &c_height);
            total_height += c_height;
        }

        for (int i = index; i < column_.getItemCount(); ++i) {
            parent_->recycleItem(column_.getItem(i));
        }
        column_.removeItems(index);

        parent_->unfreezeLayout();
    }

    int LinearListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        parent_->freezeLayout();

        int index = 0;
        int total_height = 0;
        int item_count = source_->onListGetDataCount();
        auto bounds = parent_->getContentBounds();

        int pos = cur ? cur_position_ : 0;
        int offset = cur ? cur_offset_in_position_ : 0;

        column_.setVertical(bounds.top, bounds.bottom);

        for (int i = pos; i < item_count; ++i, ++index) {
            if (total_height >= bounds.height() + offset) {
                break;
            }

            auto item = column_.getItem(index);
            DCHECK(item);

            int width = item->item_view->getDeterminedSize().width + item->getHoriMargins();
            int height = item->item_view->getDeterminedSize().height + item->getVertMargins();
            parent_->layoutItem(
                item,
                bounds.left,  bounds.top + total_height - offset,
                width, height);
            total_height += height;
        }

        parent_->unfreezeLayout();

        // 防止列表大小变化时项目超出滑动范围。
        auto last_item = column_.getLastVisible();
        auto first_item = column_.getFirstVisible();
        if (last_item && first_item && last_item->data_pos + 1 == item_count) {
            bool can_scroll = (first_item->data_pos > 0 ||
                bounds.top - first_item->getMgdTop() > 0);
            if (can_scroll) {
                return bounds.bottom - last_item->getMgdBottom();
            }
        }

        return 0;
    }

    int LinearListLayouter::onScrollToPosition(int pos, int offset, bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        int index = 0;
        int total_height = 0;
        int item_count = source_->onListGetDataCount();
        auto bounds = parent_->getContentBounds();

        pos = cur ? cur_position_ : pos;
        offset = cur ? cur_offset_in_position_ : offset;

        bool to_bottom = false;
        if (pos + 1 > item_count) {
            pos = std::max(item_count - 1, 0);
            offset = 0;
            to_bottom = true;
        }

        int diff = 0;
        bool full_child_reached = false;

        parent_->freezeLayout();

        for (int i = pos; i < item_count; ++i, ++index) {
            auto item = column_.findAndInsertItem(index, source_->onListGetItemId(i));
            if (!item || item->item_id != source_->onListGetItemId(i)) {
                item = parent_->makeNewItem(i, index);
                column_.addItem(item, index);
            } else {
                item->data_pos = i;
                source_->onListSetItemData(item, i);
            }

            int c_width, c_height;
            parent_->measureItem(item, bounds.width(), &c_width, &c_height);
            parent_->layoutItem(
                item,
                bounds.left, bounds.top + total_height - offset,
                c_width, c_height);
            total_height += c_height;

            diff = bounds.bottom - item->getMgdBottom();
            if (total_height >= bounds.height() + offset) {
                full_child_reached = true;
                ++index;
                break;
            }
        }

        for (int i = index; i < column_.getItemCount(); ++i) {
            parent_->recycleItem(column_.getItem(i));
        }
        column_.removeItems(index);

        parent_->unfreezeLayout();

        if (item_count > 0) {
            if ((!full_child_reached && diff > 0) || (to_bottom && diff < 0)) {
                return diff;
            }
        }

        return 0;
    }

    int LinearListLayouter::onSmoothScrollToPosition(int pos, int offset) {
        if (!isAvailable()) {
            return 0;
        }

        auto item_count = source_->onListGetDataCount();
        if (item_count == 0) {
            return 0;
        }
        if (pos + 1 > item_count) {
            pos = std::max(item_count - 1, 0);
            offset = 0;
        }

        int start_pos = cur_position_;
        int start_pos_offset = cur_offset_in_position_;
        int terminate_pos = pos;
        int terminate_pos_offset = offset;
        bool front = (start_pos <= terminate_pos);
        auto bounds = parent_->getContentBounds();

        int i = start_pos;
        int index = 0;

        int total_height = 0;
        bool full_child_reached = false;

        parent_->freezeLayout();

        for (; (front ? (i <= terminate_pos) : (i >= terminate_pos)); (front ? ++i : --i), ++index) {
            auto item = column_.findAndInsertItem(index, source_->onListGetItemId(i));
            if (!item || item->item_id != source_->onListGetItemId(i)) {
                item = parent_->makeNewItem(i, index);
                column_.addItem(item, index);
            } else {
                item->data_pos = i;
                source_->onListSetItemData(item, i);
            }

            int c_width, c_height;
            parent_->measureItem(item, bounds.width(), &c_width, &c_height);
            parent_->layoutItem(
                item,
                bounds.left, bounds.top + total_height + (front ? -offset : offset),
                c_width, c_height);

            if (front) {
                if (i == terminate_pos) {
                    c_height = terminate_pos_offset;
                }
                if (i == start_pos) {
                    c_height -= start_pos_offset;
                }
                if (i != start_pos && i != terminate_pos) {
                    c_height = front ? c_height : -c_height;
                }
            } else {
                if (i == start_pos) {
                    c_height = start_pos_offset;
                }
                if (i == terminate_pos) {
                    c_height -= terminate_pos_offset;
                }
                if (i != start_pos && i != terminate_pos) {
                    c_height = front ? c_height : -c_height;
                }
            }

            total_height += c_height;
        }

        parent_->unfreezeLayout();

        return -total_height;
    }

    int LinearListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto top_item = column_.getFront();
        if (!top_item) {
            return 0;
        }

        parent_->freezeLayout();

        auto bounds = parent_->getContentBounds();
        auto cur_data_pos = top_item->data_pos;

        int inc_y = 0;
        int distance_y = top_item->getMgdTop() + dy - bounds.top;
        while (cur_data_pos > 0 && !column_.isTopFilled(dy)) {
            --cur_data_pos;

            auto new_item = parent_->makeNewItem(cur_data_pos, 0);

            int c_width, c_height;
            parent_->measureItem(new_item, bounds.width(), &c_width, &c_height);
            parent_->layoutItem(
                new_item,
                bounds.left, column_.getItemsTop() - c_height,
                c_width, c_height);
            column_.addItem(new_item, 0);

            // 先行回收，防止用户快速拖动滚动条时创建出大量的 ListItem
            inc_y += c_height;
            if (inc_y <= distance_y) {
                recycleBottomChildren(inc_y);
            }
        }

        dy = column_.getFinalScroll(dy);

        recycleBottomChildren(dy);

        parent_->unfreezeLayout();
        return dy;
    }

    int LinearListLayouter::onFillBottomChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto bottom_item = column_.getRear();
        if (!bottom_item) {
            return 0;
        }

        parent_->freezeLayout();

        auto bounds = parent_->getContentBounds();
        auto cur_data_pos = bottom_item->data_pos;

        int inc_y = 0;
        int distance_y = bottom_item->getMgdBottom() + dy - bounds.bottom;
        while (cur_data_pos + 1 < source_->onListGetDataCount() && !column_.isBottomFilled(dy)) {
            ++cur_data_pos;

            auto new_item = parent_->makeNewItem(cur_data_pos, parent_->getChildCount());

            int c_width, c_height;
            parent_->measureItem(new_item, bounds.width(), &c_width, &c_height);
            parent_->layoutItem(
                new_item,
                bounds.left, column_.getItemsBottom(),
                c_width, c_height);
            column_.addItem(new_item);

            // 先行回收，防止用户快速拖动滚动条时创建出大量的 ListItem
            inc_y += c_height;
            if (inc_y <= -distance_y) {
                recycleTopChildren(-inc_y);
            }
        }

        dy = column_.getFinalScroll(dy);

        recycleTopChildren(dy);

        parent_->unfreezeLayout();
        return dy;
    }

    int LinearListLayouter::onFillLeftChildren(int dx) {
        return 0;
    }

    int LinearListLayouter::onFillRightChildren(int dx) {
        return 0;
    }

    void LinearListLayouter::onClear() {
        if (!isAvailable()) {
            return;
        }

        column_.clear();
        cur_position_ = 0;
        cur_offset_in_position_ = 0;
    }

    void LinearListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }

        auto item = column_.getFirstVisible();
        if (item) {
            cur_position_ = item->data_pos;
            cur_offset_in_position_ = parent_->getContentBounds().top - item->getMgdTop();
        } else {
            cur_position_ = 0;
            cur_offset_in_position_ = 0;
        }
    }

    void LinearListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *prev = 0; *next = 0;
            return;
        }

        auto count = source_->onListGetDataCount();
        if (count == 0) {
            *prev = 0; *next = 0;
            return;
        }

        int item_count = column_.getItemCount();
        if (item_count == 0) {
            *prev = 0; *next = 0;
            return;
        }

        // 计算当前已测量的 View 高度的平均值
        int det_height = 0;
        for (int i = 0; i < item_count; ++i) {
            auto item = column_.getItem(i);
            if (item) {
                det_height += item->getMgdHeight();
            }
        }
        int avgc_height = (det_height + item_count - 1) / item_count;

        // 计算之前的高度
        auto f_item = column_.getFront();
        auto fv_item = column_.getFirstVisible();
        if (!fv_item) {
            *prev = 0; *next = 0;
            return;
        }

        int i;
        int prev_total_height = cur_offset_in_position_ + f_item->data_pos * avgc_height;
        for (i = 0; i < fv_item->data_pos - f_item->data_pos; ++i) {
            prev_total_height += column_.getItem(i)->getMgdHeight();
        }

        // 计算之后的高度
        auto l_item = column_.getRear();

        int next_total_height = -cur_offset_in_position_ + (count - l_item->data_pos - 1) * avgc_height;
        for (; i <= l_item->data_pos - f_item->data_pos; ++i) {
            next_total_height += column_.getItem(i)->getMgdHeight();
        }

        *prev = prev_total_height;
        *next = next_total_height;
    }

    ListItem* LinearListLayouter::findItemFromView(View* v) {
        if (!isAvailable()) {
            return nullptr;
        }
        return column_.findItemFromView(v);
    }

    bool LinearListLayouter::canScroll(Direction dir) const {
        if (!isAvailable()) {
            return false;
        }

        bool result = false;
        if (dir & TOP) {
            result |= canScrollToTop();
        }
        if (dir & BOTTOM) {
            result |= canScrollToBottom();
        }
        if (dir & LEFT) {
            result |= canScrollToLeft();
        }
        if (dir & RIGHT) {
            result |= canScrollToRight();
        }
        return result;
    }

    void LinearListLayouter::getCurPosition(int* pos, int* offset) const {
        *pos = cur_position_;
        if (offset) *offset = cur_offset_in_position_;
    }

    void LinearListLayouter::recycleTopChildren(int dy) {
        int index = column_.getIndexOfFirstVisible(dy);
        if (index != -1) {
            for (int i = 0; i < index; ++i) {
                parent_->recycleItem(column_.getItem(i));
            }
            column_.removeItems(0, index - 0);
        }
    }

    void LinearListLayouter::recycleBottomChildren(int dy) {
        int index = column_.getIndexOfLastVisible(dy);
        if (index != -1) {
            ++index;
            for (int i = index; i < column_.getItemCount(); ++i) {
                parent_->recycleItem(column_.getItem(i));
            }
            column_.removeItems(index);
        }
    }

    bool LinearListLayouter::canScrollToTop() const {
        auto top_item = column_.getFront();
        if (!top_item) {
            return false;
        }
        if (top_item->data_pos == 0 && column_.atTop()) {
            return false;
        }
        return true;
    }

    bool LinearListLayouter::canScrollToBottom() const {
        auto bottom_item = column_.getRear();
        if (!bottom_item) {
            return false;
        }
        if (bottom_item->data_pos + 1 == source_->onListGetDataCount() && column_.atBottom()) {
            return false;
        }
        return true;
    }

    bool LinearListLayouter::canScrollToLeft() const {
        return false;
    }

    bool LinearListLayouter::canScrollToRight() const {
        return false;
    }

}