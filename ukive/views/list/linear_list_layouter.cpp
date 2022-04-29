// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/list/linear_list_layouter.h"

#include "utils/log.h"

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item.h"


namespace ukive {

    LinearListLayouter::LinearListLayouter()
        : cur_pos_(0),
          cur_offset_(0) {
    }

    Size LinearListLayouter::onDetermineSize(
        int cw, int ch, SizeInfo::Mode wm, SizeInfo::Mode hm)
    {
        int width = cw;
        int height = ch;

        if (!isAvailable()) {
            return Size(
                width, hm == SizeInfo::CONTENT ? 0 : height);
        }

        size_t pos = cur_pos_;
        int offset = cur_offset_;
        auto item_count = source_->onGetListDataCount(parent_);

        parent_->freezeLayout();

        size_t index = 0;
        int total_height = 0;
        bool is_filled = false;
        for (auto i = pos; i < item_count; ++i, ++index) {
            if (total_height >= height + offset) {
                is_filled = true;
                break;
            }

            auto item = column_.findAndInsertItem(
                index, source_->onGetListItemId(parent_, i));
            if (!item) {
                item = parent_->makeNewItem(i, index);
                column_.addItem(item, index);
            } else {
                if (item->data_pos != pos) {
                    parent_->setItemData(item, i);
                }
            }

            auto item_size = parent_->determineItemSize(item, width);
            total_height += item_size.height();
        }

        if (!is_filled && total_height < height) {
            for (auto i = pos; i-- > 0;) {
                auto item = parent_->makeNewItem(i, 0);
                column_.addItem(item, 0);

                auto item_size = parent_->determineItemSize(item, width);
                total_height += item_size.height();

                ++index;
                if (total_height >= height) {
                    cur_pos_ = i;
                    cur_offset_ = total_height - height;
                    break;
                }
            }
        }

        for (auto i = index; i < column_.getItemCount(); ++i) {
            parent_->recycleItem(column_.getItem(i));
        }
        column_.removeItems(index);

        parent_->unfreezeLayout();

        if (hm == SizeInfo::CONTENT && total_height < height) {
            return Size(width, total_height);
        }
        return Size(width, height);
    }

    int LinearListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        parent_->freezeLayout();

        size_t index = 0;
        int total_height = 0;
        auto item_count = source_->onGetListDataCount(parent_);
        auto bounds = parent_->getContentBounds();

        size_t pos = cur ? cur_pos_ : 0;
        int offset = cur ? cur_offset_ : 0;

        column_.setVertical(bounds.y(), bounds.bottom());

        for (auto i = pos; i < item_count; ++i, ++index) {
            if (total_height >= bounds.height() + offset) {
                break;
            }

            auto item = column_.getItem(index);
            ubassert(item);

            auto item_size = parent_->determineItemSize(item, bounds.width());
            parent_->layoutItem(
                item,
                bounds.x(),  bounds.y() + total_height - offset,
                item_size.width(), item_size.height());
            total_height += item_size.height();
        }

        parent_->unfreezeLayout();

        // 防止列表大小变化时项目超出滑动范围。
        auto last_item = column_.getLastVisible();
        auto first_item = column_.getFirstVisible();
        if (last_item && first_item && last_item->data_pos + 1 == item_count) {
            bool can_scroll = (first_item->data_pos > 0 ||
                bounds.y() - first_item->getMgdTop() > 0);
            if (can_scroll) {
                return bounds.bottom() - last_item->getMgdBottom();
            }
        }

        return 0;
    }

    int LinearListLayouter::onDataChangedAtPosition(size_t pos, int offset, bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        size_t index = 0;
        int total_height = 0;
        auto item_count = source_->onGetListDataCount(parent_);
        auto bounds = parent_->getContentBounds();

        pos = cur ? cur_pos_ : pos;
        offset = cur ? cur_offset_ : offset;

        bool to_bottom = false;
        if (pos + 1 > item_count) {
            pos = item_count > 0 ? item_count - 1 : 0;
            offset = 0;
            to_bottom = true;
        }

        int diff = 0;
        bool full_child_reached = false;

        parent_->freezeLayout();

        for (auto i = pos; i < item_count; ++i, ++index) {
            auto item = column_.findAndInsertItem(index, source_->onGetListItemId(parent_, i));
            if (!item || item->item_id != source_->onGetListItemId(parent_, i)) {
                item = parent_->makeNewItem(i, index);
                column_.addItem(item, index);
            } else {
                parent_->setItemData(item, i);
            }

            auto item_size = parent_->determineItemSize(item, bounds.width());
            parent_->layoutItem(
                item,
                bounds.x(), bounds.y() + total_height - offset,
                item_size.width(), item_size.height());
            total_height += item_size.height();

            diff = bounds.bottom() - item->getMgdBottom();
            if (total_height >= bounds.height() + offset) {
                full_child_reached = true;
                ++index;
                break;
            }
        }

        for (auto i = index; i < column_.getItemCount(); ++i) {
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

    int LinearListLayouter::onSmoothScrollToPosition(size_t pos, int offset) {
        if (!isAvailable()) {
            return 0;
        }

        auto item_count = source_->onGetListDataCount(parent_);
        if (item_count == 0) {
            return 0;
        }
        if (pos + 1 > item_count) {
            pos = item_count > 0 ? item_count - 1 : 0;
            offset = 0;
        }

        auto start_pos = cur_pos_;
        auto start_pos_offset = cur_offset_;
        auto terminate_pos = pos;
        auto terminate_pos_offset = offset;
        bool front = (start_pos <= terminate_pos);
        auto bounds = parent_->getContentBounds();

        auto i = start_pos;
        size_t index = 0;
        int total_height = 0;

        parent_->freezeLayout();

        for (; (front ? (i <= terminate_pos) : (i-- > terminate_pos)); (front ? ++i : 0), ++index) {
            auto item = column_.findAndInsertItem(index, source_->onGetListItemId(parent_, i));
            if (!item || item->item_id != source_->onGetListItemId(parent_, i)) {
                item = parent_->makeNewItem(i, index);
                column_.addItem(item, index);
            } else {
                parent_->setItemData(item, i);
            }

            auto item_size = parent_->determineItemSize(item, bounds.width());
            parent_->layoutItem(
                item,
                bounds.x(), bounds.y() + total_height + (front ? -offset : offset),
                item_size.width(), item_size.height());

            if (front) {
                if (i == terminate_pos) {
                    item_size.height(terminate_pos_offset);
                }
                if (i == start_pos) {
                    item_size.height(item_size.height() - start_pos_offset);
                }
                if (i != start_pos && i != terminate_pos) {
                    item_size.height(front ? item_size.height() : -item_size.height());
                }
            } else {
                if (i == start_pos) {
                    item_size.height(start_pos_offset);
                }
                if (i == terminate_pos) {
                    item_size.height(item_size.height() - terminate_pos_offset);
                }
                if (i != start_pos && i != terminate_pos) {
                    item_size.height(front ? item_size.height() : -item_size.height());
                }
            }

            total_height += item_size.height();
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
        int distance_y = top_item->getMgdTop() + dy - bounds.y();
        while (cur_data_pos > 0 && !column_.isTopFilled(dy)) {
            --cur_data_pos;

            auto new_item = parent_->makeNewItem(cur_data_pos, 0);

            auto item_size = parent_->determineItemSize(new_item, bounds.width());
            parent_->layoutItem(
                new_item,
                bounds.x(), column_.getItemsTop() - item_size.height(),
                item_size.width(), item_size.height());
            column_.addItem(new_item, 0);

            // 先行回收，防止用户快速拖动滚动条时创建出大量的 ListItem
            inc_y += item_size.height();
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
        int distance_y = bottom_item->getMgdBottom() + dy - bounds.bottom();
        while (cur_data_pos + 1 < source_->onGetListDataCount(parent_) && !column_.isBottomFilled(dy)) {
            ++cur_data_pos;

            auto new_item = parent_->makeNewItem(cur_data_pos, parent_->getChildCount());

            auto item_size = parent_->determineItemSize(new_item, bounds.width());
            parent_->layoutItem(
                new_item,
                bounds.x(), column_.getItemsBottom(),
                item_size.width(), item_size.height());
            column_.addItem(new_item);

            // 先行回收，防止用户快速拖动滚动条时创建出大量的 ListItem
            inc_y += item_size.height();
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
        column_.clear();
        cur_pos_ = 0;
        cur_offset_ = 0;
    }

    void LinearListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }

        auto item = column_.getFirstVisible();
        if (item) {
            cur_pos_ = item->data_pos;
            cur_offset_ = parent_->getContentBounds().y() - item->getMgdTop();
        } else {
            cur_pos_ = 0;
            cur_offset_ = 0;
        }
    }

    void LinearListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *prev = 0; *next = 0;
            return;
        }

        auto count = source_->onGetListDataCount(parent_);
        if (count == 0) {
            *prev = 0; *next = 0;
            return;
        }

        auto item_count = column_.getItemCount();
        if (item_count == 0) {
            *prev = 0; *next = 0;
            return;
        }

        // 计算当前已测量的 View 高度的平均值
        int det_height = 0;
        for (size_t i = 0; i < item_count; ++i) {
            auto item = column_.getItem(i);
            if (item) {
                det_height += item->getMgdHeight();
            }
        }
        int avgc_height = int((det_height + item_count - 1) / item_count);

        // 计算之前的高度
        auto f_item = column_.getFront();
        auto fv_item = column_.getFirstVisible();
        if (!fv_item) {
            *prev = 0; *next = 0;
            return;
        }

        size_t i;
        int prev_total_height = int(cur_offset_ + f_item->data_pos * avgc_height);
        for (i = 0; i < fv_item->data_pos - f_item->data_pos; ++i) {
            prev_total_height += column_.getItem(i)->getMgdHeight();
        }

        // 计算之后的高度
        auto l_item = column_.getRear();

        int next_total_height = int(-cur_offset_ + (count - l_item->data_pos - 1) * avgc_height);
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

    void LinearListLayouter::getCurPosition(size_t* pos, int* offset) const {
        *pos = cur_pos_;
        if (offset) *offset = cur_offset_;
    }

    void LinearListLayouter::recycleTopChildren(int dy) {
        size_t index;
        if (column_.getIndexOfFirstVisible(dy, &index)) {
            for (size_t i = 0; i < index; ++i) {
                parent_->recycleItem(column_.getItem(i));
            }
            column_.removeItems(0, index - 0);
        }
    }

    void LinearListLayouter::recycleBottomChildren(int dy) {
        size_t index;
        if (column_.getIndexOfLastVisible(dy, &index)) {
            ++index;
            for (auto i = index; i < column_.getItemCount(); ++i) {
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
        if (bottom_item->data_pos + 1 == source_->onGetListDataCount(parent_) && column_.atBottom()) {
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