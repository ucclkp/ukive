// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/list/grid_list_layouter.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/list_item.h"
#include "ukive/views/list/list_item_recycler.h"


namespace ukive {

    GridListLayouter::GridListLayouter(size_t col_count)
        : col_count_(col_count),
          columns_(col_count_)
    {
    }

    Size GridListLayouter::onDetermineSize(
        int cw, int ch, SizeInfo::Mode wm, SizeInfo::Mode hm)
    {
        int width = cw;
        int height = ch;

        if (!isAvailable()) {
            return Size(
                width, hm == SizeInfo::CONTENT ? 0 : height);
        }

        size_t cur_row = cur_row_;
        int offset = cur_offset_;
        auto item_count = source_->onGetListDataCount(parent_);

        columns_.setHorizontal(0, width);
        parent_->freezeLayout();

        size_t row_index = 0;
        size_t view_index = 0;
        int total_height = 0;
        size_t cur_col = 0;
        bool is_filled = false;
        for (auto row = cur_row; row * col_count_ < item_count; ++row) {
            if (total_height >= height + offset) {
                is_filled = true;
                break;
            }

            size_t col;
            int row_height = 0;
            for (col = 0; col < col_count_; ++col) {
                auto pos = row * col_count_ + col;
                if (pos >= item_count) {
                    break;
                }

                auto item = columns_[col].findAndInsertItem(
                    row_index, source_->onGetListItemId(parent_, pos));
                if (!item) {
                    item = parent_->makeNewItem(pos, view_index);
                    columns_[col].addItem(item, row_index);
                } else {
                    if (item->data_pos != pos) {
                        parent_->setItemData(item, pos);
                    }
                }

                int child_max_width = columns_[col].getWidth();
                auto item_size = parent_->determineItemSize(item, child_max_width);
                row_height = (std::max)(item_size.height(), row_height);

                ++view_index;
            }

            ++row_index;
            total_height += row_height;
            // 一行填不满的话，记下到哪一列为止
            cur_col = col < col_count_ ? col : 0;
        }

        if (!is_filled && total_height < height) {
            for (auto row = cur_row; row-- > 0;) {
                int row_height = 0;
                for (size_t col = 0; col < col_count_; ++col) {
                    auto pos = row * col_count_ + col;
                    auto item = parent_->makeNewItem(pos, col);
                    columns_[col].addItem(item, 0);

                    int child_max_width = columns_[col].getWidth();
                    auto item_size = parent_->determineItemSize(item, child_max_width);
                    row_height = (std::max)(item_size.height(), row_height);
                }

                ++row_index;
                total_height += row_height;
                if (total_height >= height) {
                    cur_row_ = row;
                    cur_offset_ = total_height - height;
                    break;
                }
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            auto start = row_index;
            // 如果存在未填满的行，要把空位留出来
            if (cur_col > 0 && i >= cur_col) {
                --start;
            }

            auto count = columns_[i].getItemCount();
            for (auto j = start; j < count; ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(start);
        }

        parent_->unfreezeLayout();

        if (hm == SizeInfo::CONTENT && total_height < height) {
            return Size(width, total_height);
        }
        return Size(width, height);
    }

    int GridListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        size_t cur_row = cur ? cur_row_ : 0;
        int offset = cur ? cur_offset_ : 0;
        auto item_count = source_->onGetListDataCount(parent_);
        auto bounds = parent_->getContentBounds();

        columns_.setVertical(bounds.y(), bounds.bottom());
        columns_.setHorizontal(bounds.x(), bounds.right());

        parent_->freezeLayout();

        size_t row_index = 0;
        size_t view_index = 0;
        int total_height = 0;
        size_t cur_col = 0;
        for (auto row = cur_row; row * col_count_ < item_count; ++row) {
            size_t col;
            int row_height = 0;
            for (col = 0; col < col_count_; ++col) {
                auto pos = row * col_count_ + col;
                if (pos >= item_count) {
                    break;
                }

                auto item = columns_[col].getItem(row_index);
                ubassert(item);

                int width = item->item_view->getDeterminedSize().width() + item->getHoriMargins();
                int height = item->item_view->getDeterminedSize().height() + item->getVertMargins();
                parent_->layoutItem(
                    item,
                    columns_[col].getLeft(), bounds.y() + total_height - offset,
                    width, height);
                row_height = (std::max)(height, row_height);

                ++view_index;
            }

            ++row_index;
            total_height += row_height;
            // 一行填不满的话，记下到哪一列为止
            cur_col = col < col_count_ ? col : 0;

            if (total_height >= bounds.height() + offset) {
                break;
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            auto start = row_index;
            // 如果存在未填满的行，要把空位留出来
            if (cur_col > 0 && i >= cur_col) {
                --start;
            }

            auto count = columns_[i].getItemCount();
            for (auto j = start; j < count; ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(start);
        }

        parent_->unfreezeLayout();

        // 防止列表大小变化时项目超出滑动范围。
        if (columns_.isAllAtFloor(item_count)) {
            bool is_at_ceil = columns_.isAllAtCeil(item_count);
            bool can_scroll = !is_at_ceil;
            if (is_at_ceil) {
                auto topmost = columns_.getTopmost();
                if (topmost) {
                    can_scroll = (bounds.y() - topmost->getY() > 0);
                }
            }
            if (can_scroll) {
                return bounds.bottom() - columns_.getBottomost()->getBottom();
            }
        }

        return 0;
    }

    int GridListLayouter::onDataChangedAtPosition(size_t pos, int offset, bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        auto item_count = source_->onGetListDataCount(parent_);
        auto bounds = parent_->getContentBounds();

        size_t cur_row = cur ? cur_row_ : pos / col_count_;
        offset = cur ? cur_offset_ : offset;

        bool to_bottom = false;
        size_t row_count = (item_count + (col_count_ - 1)) / col_count_;
        if (cur_row >= row_count) {
            cur_row = row_count > 0 ? row_count - 1 : 0;
            offset = 0;
            to_bottom = true;
        }

        int diff = 0;
        bool is_filled = false;

        parent_->freezeLayout();

        size_t row_index = 0;
        size_t view_index = 0;
        int total_height = 0;
        size_t cur_col = 0;
        for (auto row = cur_row; row * col_count_ < item_count; ++row) {
            size_t col;
            int row_height = 0;
            for (col = 0; col < col_count_; ++col) {
                auto i_pos = row * col_count_ + col;
                if (i_pos >= item_count) {
                    break;
                }

                auto item = columns_[col].findAndInsertItem(
                    row_index, source_->onGetListItemId(parent_, i_pos));
                if (!item) {
                    item = parent_->makeNewItem(i_pos, view_index);
                    columns_[col].addItem(item, row_index);
                } else {
                    parent_->setItemData(item, i_pos);
                }

                int child_max_width = columns_[col].getWidth();

                auto item_size = parent_->determineItemSize(item, child_max_width);
                parent_->layoutItem(
                    item,
                    columns_[col].getLeft(), bounds.y() + total_height - offset,
                    item_size.width(), item_size.height());
                row_height = (std::max)(item_size.height(), row_height);

                ++view_index;
            }

            ++row_index;
            total_height += row_height;
            cur_col = col < col_count_ ? col : 0;

            diff = bounds.bottom() - (bounds.y() + total_height - offset);
            if (total_height >= bounds.height() + offset) {
                is_filled = true;
                break;
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            auto start = row_index;
            if (cur_col > 0 && i >= cur_col) {
                --start;
            }

            auto count = columns_[i].getItemCount();
            for (auto j = start; j < count; ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(start);
        }

        parent_->unfreezeLayout();

        if (row_count > 0) {
            if ((!is_filled && diff > 0) || (to_bottom && diff < 0)) {
                return diff;
            }
        }

        return 0;
    }

    int GridListLayouter::onSmoothScrollToPosition(size_t pos, int offset) {
        return 0;
    }

    int GridListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto top_item = columns_.getFirst();
        if (!top_item) {
            return 0;
        }
        auto bounds = parent_->getContentBounds();
        auto cur_data_pos = top_item->data_pos;

        parent_->freezeLayout();

        int inc_y = 0;
        int distance_y = getColsTop() + dy - bounds.y();
        while (cur_data_pos > 0 && !columns_.isTopFilled(dy)) {
            --cur_data_pos;
            auto col = cur_data_pos % col_count_;

            ubassert(col + 1 == col_count_);

            auto tmp_pos = cur_data_pos;
            std::vector<ListItem*> tmps;

            int max_height = 0;
            for (size_t i = col_count_; i-- > 0;) {
                int child_max_width = columns_[i].getWidth();
                auto new_item = parent_->makeNewItem(tmp_pos, 0);

                auto item_size = parent_->determineItemSize(new_item, child_max_width);

                max_height = (std::max)(max_height, item_size.height());
                tmps.push_back(new_item);

                --tmp_pos;
            }

            int cur_top = columns_[0].getItemsTop() - max_height;
            for (size_t i = col_count_; i-- > 0;) {
                auto new_item = tmps[col_count_ - 1 - i];
                int child_width = columns_[i].getWidth();
                int height = new_item->item_view->getDeterminedSize().height() + new_item->getVertMargins();
                parent_->layoutItem(
                    new_item,
                    columns_[i].getLeft(), cur_top,
                    child_width, height);
                columns_[i].addItem(new_item, 0);

                if (i > 0) {
                    --cur_data_pos;
                }
            }

            // 先行回收，防止用户快速拖动滚动条时创建出大量的 ListItem
            inc_y += max_height;
            if (inc_y <= distance_y) {
                recycleBottomChildren(inc_y);
            }
        }

        dy = columns_.getFinalScroll(dy);

        recycleBottomChildren(dy);

        parent_->unfreezeLayout();

        return dy;
    }

    int GridListLayouter::onFillBottomChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto bottom_item = columns_.getLast();
        if (!bottom_item) {
            return 0;
        }
        auto bounds = parent_->getContentBounds();
        auto cur_data_pos = bottom_item->data_pos;

        parent_->freezeLayout();

        int inc_y = 0;
        int distance_y = getColsBottom() + dy - bounds.bottom();
        while (cur_data_pos + 1 < source_->onGetListDataCount(parent_) && !columns_.isBottomOneFilled(dy)) {
            ++cur_data_pos;
            auto row = cur_data_pos / col_count_;
            auto col = cur_data_pos % col_count_;

            ubassert(col == 0);

            int prev_bottom = 0;
            if (row > 0) {
                auto prev_index = columns_[col].getItemCount();
                if (prev_index > 0) {
                    --prev_index;
                    for (size_t i = 0; i < col_count_; ++i) {
                        prev_bottom = (std::max)(prev_bottom, columns_[i].getItem(prev_index)->getBottom());
                    }
                }
            }

            int max_height = 0;
            for (auto i = col; i < col_count_; ++i) {
                int child_max_width = columns_[i].getWidth();
                auto new_item = parent_->makeNewItem(cur_data_pos, parent_->getChildCount());

                auto item_size = parent_->determineItemSize(new_item, child_max_width);
                parent_->layoutItem(
                    new_item,
                    columns_[i].getLeft(), prev_bottom,
                    item_size.width(), item_size.height());
                columns_[i].addItem(new_item);

                if (item_size.height() > max_height) {
                    max_height = item_size.height();
                }

                if (i + 1 < col_count_) {
                    ++cur_data_pos;
                    if (cur_data_pos >= source_->onGetListDataCount(parent_)) {
                        break;
                    }
                }
            }

            // 先行回收，防止用户快速拖动滚动条时创建出大量的 ListItem
            inc_y += max_height;
            if (inc_y <= -distance_y) {
                recycleTopChildren(-inc_y);
            }
        }

        dy = columns_.getFinalScroll(dy);

        recycleTopChildren(dy);

        parent_->unfreezeLayout();

        return dy;
    }

    int GridListLayouter::onFillLeftChildren(int dx) {
        return 0;
    }

    int GridListLayouter::onFillRightChildren(int dx) {
        return 0;
    }

    void GridListLayouter::onClear() {
        columns_.clear();
    }

    void GridListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *next = *prev = 0;
            return;
        }

        auto item_count = source_->onGetListDataCount(parent_);
        if (item_count == 0) {
            *next = *prev = 0;
            return;
        }

        const auto count = columns_[0].getItemCount();

        static std::vector<int> row_heights;
        row_heights.clear();

        int avg_height = 0;
        for (size_t i = 0; i < count; ++i) {
            int row_height = 0;
            for (size_t j = 0; j < col_count_; ++j) {
                auto item = columns_[j].getItem(i);
                if (item) {
                    row_height = (std::max)(row_height, item->getHeight());
                }
            }
            row_heights.push_back(row_height);
            avg_height += row_height;
        }

        int child_height;
        if (count == 0) {
            child_height = 0;
        } else {
            child_height = int(avg_height / count);
        }

        // 计算之前的高度
        size_t fv_item_idx = 0;
        {
            bool is_first = true;
            for (size_t i = 0; i < col_count_; ++i) {
                size_t index;
                if (columns_[i].getIndexOfFirstVisible(0, &index)) {
                    if (is_first) {
                        fv_item_idx = index;
                        is_first = false;
                    } else {
                        if (index < fv_item_idx) {
                            fv_item_idx = index;
                        }
                    }
                }
            }
        }

        size_t i;
        int prev_total_height = int(cur_offset_ + cur_row_ * child_height);
        for (i = 0; i < fv_item_idx - 0; ++i) {
            prev_total_height += row_heights[i];
        }

        auto row_count = item_count / col_count_;
        if (item_count % col_count_) {
            ++row_count;
        }

        // 计算之后的高度
        auto l_item_end = count;

        int next_total_height = int(-cur_offset_
            + (row_count - cur_row_ - (l_item_end - fv_item_idx)) * child_height);
        for (; i < l_item_end - fv_item_idx; ++i) {
            next_total_height += row_heights[i];
        }

        *prev = prev_total_height;
        *next = next_total_height;
    }

    ListItem* GridListLayouter::findItemFromView(View* v) {
        if (!isAvailable()) {
            return nullptr;
        }
        return columns_.findItemFromView(v);
    }

    void GridListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }
        auto bounds = parent_->getContentBounds();
        if (bounds.empty()) {
            return;
        }

        size_t tmp_pos = 0;
        int tmp_offset = 0;
        bool is_first = true;
        for (size_t i = 0; i < col_count_; ++i) {
            auto item = columns_[i].getFirstVisible();
            if (item) {
                if (is_first) {
                    tmp_pos = item->data_pos;
                    tmp_offset = bounds.y() - item->getY();
                    is_first = false;
                } else {
                    if (item->data_pos < tmp_pos) {
                        tmp_pos = item->data_pos;
                        tmp_offset = bounds.y() - item->getY();
                    }
                }
            }
        }

        cur_row_ = tmp_pos / col_count_;
        cur_offset_ = tmp_offset;
    }

    bool GridListLayouter::canScroll(Direction dir) const {
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

    void GridListLayouter::getCurPosition(size_t* pos, int* offset) const {
        *pos = cur_row_ * col_count_;
        if (offset) *offset = cur_offset_;
    }

    void GridListLayouter::recycleTopChildren(int dy) {
        size_t min_col = 0;
        size_t min_data_pos = (std::numeric_limits<size_t>::max)();
        for (size_t i = 0; i < col_count_; ++i) {
            auto item = columns_[i].getFirstVisible(dy);
            if (item && item->data_pos < min_data_pos) {
                min_col = i;
                min_data_pos = item->data_pos;
            }
        }

        size_t index = 0;
        columns_[min_col].getIndexOfFirstVisible(dy, &index);
        if (index > 0) {
            for (size_t i = 0; i < col_count_; ++i) {
                for (size_t j = 0; j < index; ++j) {
                    parent_->recycleItem(columns_[i].getItem(j));
                }
                columns_[i].removeItems(0, index - 0);
            }
        }
    }

    void GridListLayouter::recycleBottomChildren(int dy) {
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
    }

    int GridListLayouter::getColsTop() const {
        auto front = columns_[0].getFront();
        if (front) {
            return front->getY();
        }
        return 0;
    }

    int GridListLayouter::getColsBottom() const {
        int bottom = 0;
        for (size_t i = 0; i < col_count_; ++i) {
            auto rear = columns_[i].getRear();
            if (!rear) {
                continue;
            }

            if (rear->getBottom() > bottom) {
                bottom = rear->getBottom();
            }
        }
        return bottom;
    }

    bool GridListLayouter::canScrollToTop() const {
        auto item_count = source_->onGetListDataCount(parent_);
        return !(columns_.isAllAtTop() && columns_.isAllAtCeil(item_count));
    }

    bool GridListLayouter::canScrollToBottom() const {
        auto item_count = source_->onGetListDataCount(parent_);
        return !(columns_.isAllAtBottom() && columns_.isAllAtFloor(item_count));
    }

    bool GridListLayouter::canScrollToLeft() const {
        return false;
    }

    bool GridListLayouter::canScrollToRight() const {
        return false;
    }

}
