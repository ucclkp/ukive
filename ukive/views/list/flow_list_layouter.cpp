// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/list/flow_list_layouter.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item_recycler.h"

namespace ukive {

    FlowListLayouter::FlowListLayouter()
        : col_count_(4),
          columns_(col_count_)
    {
        cur_records_.resize(col_count_, {0, 0, false});
    }

    void FlowListLayouter::onMeasureAtPosition(bool cur, int width, int height) {
        if (!isAvailable()) {
            return;
        }

        auto pos = calPreferredCurPos();
        auto item_count = source_->onListGetDataCount();

        std::vector<size_t> indices(col_count_, 0);
        std::vector<int> heights(col_count_, 0);
        columns_.setVertical(0, height);
        columns_.setHorizontal(0, width);

        parent_->freezeLayout();

        for (auto& r : cur_records_) {
            if (!r.is_null) {
                r.cur_offset = std::max(r.cur_offset, 0);
            }
        }

        size_t index = 0;
        for (auto i = pos; i < item_count; ++i) {
            auto row = i / col_count_;
            auto col = i % col_count_;
            const auto& record = cur_records_[col];
            if (record.is_null) {
                continue;
            }
            if (record.cur_row > row) {
                continue;
            }
            if (heights[col] >= height + record.cur_offset) {
                continue;
            }

            auto item = columns_[col].findAndInsertItem(indices[col], source_->onListGetItemId(i));
            if (!item) {
                item = parent_->makeNewItem(i, index);
                columns_[col].addItem(item, indices[col]);
            } else {
                item->data_pos = i;
                source_->onListSetItemData(item, i);
            }

            int child_max_width = columns_[col].getWidth();

            int c_width, c_height;
            parent_->measureItem(item, child_max_width, &c_width, &c_height);
            heights[col] += c_height;

            ++index;
            ++indices[col];

            bool is_filled = true;
            for (size_t j = 0; j < col_count_; ++j) {
                if (heights[j] < height + cur_records_[j].cur_offset) {
                    is_filled = false;
                    break;
                }
            }
            if (is_filled) {
                break;
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            for (size_t j = indices[i]; j < columns_[i].getItemCount(); ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(indices[i]);
        }

        parent_->unfreezeLayout();
    }

    int FlowListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        auto pos = calPreferredCurPos();
        auto item_count = source_->onListGetDataCount();
        auto bounds = parent_->getContentBounds();

        size_t index = 0;
        std::vector<size_t> indices(col_count_, 0);
        std::vector<int> heights(col_count_, 0);
        columns_.setVertical(bounds.top, bounds.bottom);
        columns_.setHorizontal(bounds.left, bounds.right);

        parent_->freezeLayout();

        for (auto& r : cur_records_) {
            if (!r.is_null) {
                r.cur_offset = std::max(r.cur_offset, 0);
            }
        }

        for (auto i = pos; i < item_count; ++i) {
            auto row = i / col_count_;
            auto col = i % col_count_;
            const auto& record = cur_records_[col];
            if (record.is_null) {
                continue;
            }
            if (record.cur_row > row) {
                continue;
            }
            if (heights[col] >= bounds.height() + record.cur_offset) {
                continue;
            }

            auto item = columns_[col].getItem(indices[col]);
            DCHECK(item);

            int width = item->item_view->getDeterminedSize().width + item->getHoriMargins();
            int height = item->item_view->getDeterminedSize().height + item->getVertMargins();
            parent_->layoutItem(
                item,
                columns_[col].getLeft(), bounds.top + heights[col] - record.cur_offset,
                width, height);
            heights[col] += height;

            ++index;
            ++indices[col];

            bool is_filled = true;
            for (size_t j = 0; j < col_count_; ++j) {
                if (heights[j] < bounds.height() + cur_records_[j].cur_offset) {
                    is_filled = false;
                    break;
                }
            }
            if (is_filled) {
                break;
            }
        }

        parent_->unfreezeLayout();

        // 防止列表大小变化时项目超出滑动范围。
        if (columns_.isAllAtFloor(item_count)) {
            bool is_at_ceil = columns_.isAllAtCeil(item_count);
            bool can_scroll = !is_at_ceil;
            if (is_at_ceil) {
                auto topmost = columns_.getTopmost();
                if (topmost) {
                    can_scroll = (bounds.top - topmost->getMgdTop() > 0);
                }
            }
            if (can_scroll) {
                return bounds.bottom - columns_.getBottomost()->getMgdBottom();
            }
        }

        return 0;
    }

    int FlowListLayouter::onScrollToPosition(size_t pos, int offset, bool cur) {
        return 0;
    }

    int FlowListLayouter::onSmoothScrollToPosition(size_t pos, int offset) {
        return 0;
    }

    int FlowListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto top_item = columns_.getTopStart();
        if (!top_item) {
            return 0;
        }

        auto cur_data_pos = top_item->data_pos;

        size_t cur_index;
        bool found_view = parent_->findViewIndexFromStart(top_item, &cur_index);
        parent_->freezeLayout();

        while (cur_data_pos > 0 && !columns_.isTopFilled(dy)) {
            --cur_data_pos;
            auto row = cur_data_pos / col_count_;
            auto col = cur_data_pos % col_count_;
            if (columns_[col].isTopFilled(dy)) {
                continue;
            }

            auto cur_item = columns_.getItemByPos(cur_data_pos);
            if (cur_item) {
                found_view = parent_->findViewIndexFromStart(cur_item, &cur_index);
                continue;
            }

            DCHECK(found_view);

            int child_max_width = columns_[col].getWidth();
            auto new_item = parent_->makeNewItem(cur_data_pos, cur_index);

            int c_width, c_height;
            parent_->measureItem(new_item, child_max_width, &c_width, &c_height);
            parent_->layoutItem(
                new_item,
                columns_[col].getLeft(), columns_[col].getItemsTop() - c_height,
                c_width, c_height);
            columns_[col].addItem(new_item, 0);
        }

        dy = columns_.getFinalScroll(dy);

        for (size_t i = 0; i < col_count_; ++i) {
            size_t index;
            if (columns_[i].getIndexOfLastVisible(dy, &index)) {
                ++index;
                for (auto j = index; j < columns_[i].getItemCount(); ++j) {
                    parent_->recycleItem(columns_[i].getItem(j));
                }
                columns_[i].removeItems(index);
            }
        }

        parent_->unfreezeLayout();

        return dy;
    }

    int FlowListLayouter::onFillBottomChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto bottom_item = columns_.getBottomStart();
        if (!bottom_item) {
            return 0;
        }
        auto bounds = parent_->getContentBounds();
        auto cur_data_pos = bottom_item->data_pos;

        size_t cur_index;
        bool found_view = parent_->findViewIndexFromEnd(bottom_item, &cur_index);

        parent_->freezeLayout();

        while (cur_data_pos + 1 < source_->onListGetDataCount() && !columns_.isBottomFilled(dy)) {
            ++cur_data_pos;
            int row = cur_data_pos / col_count_;
            int col = cur_data_pos % col_count_;
            if (columns_[col].isBottomFilled(dy)) {
                continue;
            }

            auto cur_item = columns_.getItemByPos(cur_data_pos);
            if (cur_item) {
                found_view = parent_->findViewIndexFromEnd(cur_item, &cur_index);
                continue;
            }

            DCHECK(found_view);

            int child_max_width = columns_[col].getWidth();
            auto new_item = parent_->makeNewItem(cur_data_pos, cur_index + 1);

            int c_width, c_height;
            parent_->measureItem(new_item, child_max_width, &c_width, &c_height);
            parent_->layoutItem(
                new_item,
                columns_[col].getLeft(), columns_[col].getItemsBottom(),
                c_width, c_height);
            columns_[col].addItem(new_item);
            ++cur_index;
        }

        dy = columns_.getFinalScroll(dy);

        for (size_t i = 0; i < col_count_; ++i) {
            size_t index;
            if (columns_[i].getIndexOfFirstVisible(dy, &index)) {
                for (size_t j = 0; j < index; ++j) {
                    parent_->recycleItem(columns_[i].getItem(j));
                }
                columns_[i].removeItems(0, index - 0);
            }
        }

        parent_->unfreezeLayout();

        return dy;
    }

    int FlowListLayouter::onFillLeftChildren(int dx) {
        return 0;
    }

    int FlowListLayouter::onFillRightChildren(int dx) {
        return 0;
    }

    void FlowListLayouter::onClear() {
        if (!isAvailable()) {
            return;
        }

        columns_.clear();
    }

    void FlowListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *next = *prev = 0;
            return;
        }

        auto data_count = source_->onListGetDataCount();
        if (data_count == 0) {
            *next = *prev = 0;
            return;
        }

        size_t max_col = 0;
        size_t max_col_count = 0;
        int max_col_height = 0;
        for (size_t i = 0; i < col_count_; ++i) {
            int avg_height = 0;
            auto count = columns_[i].getItemCount();
            for (size_t j = 0; j < count; ++j) {
                auto item = columns_[i].getItem(j);
                avg_height += item->getMgdHeight();
            }

            if (avg_height > max_col_height) {
                max_col = i;
                max_col_height = avg_height;
                max_col_count = count;
            }
        }
        int child_height;
        if (max_col_count == 0) {
            child_height = 0;
        } else {
            child_height = max_col_height / max_col_count;
        }

        auto cur_row = cur_records_[max_col].cur_row;
        auto cur_offset_in_row = cur_records_[max_col].cur_offset;

        size_t i;
        int prev_total_height = cur_offset_in_row;
        for (i = 0; i < cur_row; ++i) {
            prev_total_height += child_height;
        }

        size_t row_count = data_count / col_count_;
        if (data_count % col_count_) {
            ++row_count;
        }

        int next_total_height = -cur_offset_in_row;
        for (; i < row_count; ++i) {
            next_total_height += child_height;
        }

        *prev = prev_total_height;
        *next = next_total_height;
    }

    ListItem* FlowListLayouter::findItemFromView(View* v) {
        if (!isAvailable()) {
            return nullptr;
        }
        return columns_.findItemFromView(v);
    }

    void FlowListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }
        auto bounds = parent_->getContentBounds();
        if (bounds.empty()) {
            return;
        }

        for (size_t i = 0; i < col_count_; ++i) {
            Record record;
            auto item = columns_[i].getFirstVisible();
            if (!item) {
                item = columns_[i].getRear();
            }
            if (item) {
                record.is_null = false;
                record.cur_row = item->data_pos / col_count_;
                record.cur_offset = bounds.top - item->getMgdTop();
                //DCHECK(record.cur_offset >= 0);
            }
            cur_records_[i] = record;
        }
    }

    bool FlowListLayouter::canScroll(Direction dir) const {
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

    void FlowListLayouter::getCurPosition(size_t* pos, int* offset) const {
        *pos = 0;
        if (offset) *offset = 0;
    }

    size_t FlowListLayouter::calPreferredCurPos() const {
        size_t index = 0;
        size_t pos = std::numeric_limits<size_t>::max();
        for (const auto& r : cur_records_) {
            if (r.is_null) {
                ++index;
                continue;
            }
            pos = std::min(pos, r.cur_row * col_count_ + index);
            if (pos == 0) {
                break;
            }
            ++index;
        }
        return pos;
    }

    bool FlowListLayouter::canScrollToTop() const {
        auto item_count = source_->onListGetDataCount();
        return !(columns_.isAllAtTop() && columns_.isAllAtCeil(item_count));
    }

    bool FlowListLayouter::canScrollToBottom() const {
        auto item_count = source_->onListGetDataCount();
        return !(columns_.isAllAtBottom() && columns_.isAllAtFloor(item_count));
    }

    bool FlowListLayouter::canScrollToLeft() const {
        return false;
    }

    bool FlowListLayouter::canScrollToRight() const {
        return false;
    }

}
