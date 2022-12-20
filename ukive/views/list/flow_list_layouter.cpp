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


namespace {

    template <typename Ty>
    bool findNext(Ty start, const std::vector<Ty>& vec, Ty* out) {
        auto _vec = vec;
        auto target = start;
        std::sort(_vec.begin(), _vec.end());

        for (const auto& v : _vec) {
            if (v > target) {
                if (target == (std::numeric_limits<Ty>::max)()) return false;
                ++target;
                if (v > target) {
                    *out = target;
                    return true;
                }
            }
        }

        if (target == (std::numeric_limits<Ty>::max)()) return false;
        *out = ++target;
        return true;
    }

    template <typename Ty>
    bool findNext2(Ty start, const std::vector<Ty>& vec, Ty* out) {
        auto it = std::max_element(vec.begin(), vec.end());
        if (it == vec.end()) {
            return false;
        }
        if (*it == (std::numeric_limits<Ty>::max)()) return false;
        *out = (*it) + 1;
        return true;
    }

    template <typename Ty>
    bool findPrev(Ty start, const std::vector<Ty>& vec, Ty* out) {
        auto _vec = vec;
        auto target = start;
        std::sort(_vec.begin(), _vec.end(), std::greater<>{});

        for (const auto& v : _vec) {
            if (v < target) {
                if (target == (std::numeric_limits<Ty>::min)()) return false;
                --target;
                if (v < target) {
                    *out = target;
                    return true;
                }
            }
        }

        if (target == (std::numeric_limits<Ty>::min)()) return false;
        *out = --target;
        return true;
    }

    template <typename Ty>
    bool findPrev2(Ty start, const std::vector<Ty>& vec, Ty* out) {
        auto it = std::min_element(vec.begin(), vec.end());
        if (it == vec.end()) {
            return false;
        }
        if (*it == (std::numeric_limits<Ty>::min)()) return false;
        *out = (*it) - 1;
        return true;
    }

}

namespace ukive {

    FlowListLayouter::FlowListLayouter(size_t col_count)
        : col_count_(col_count),
          columns_(col_count_)
    {
        cur_records_.resize(col_count_, {0, 0, false});
        for (size_t i = 0; i < col_count_; ++i) {
            auto& r = cur_records_[i];
            r.cur_data_pos = i;
        }
    }

    Size FlowListLayouter::onDetermineSize(
        int cw, int ch, SizeInfo::Mode wm, SizeInfo::Mode hm)
    {
        int width = cw;
        int height = ch;

        if (!isAvailable()) {
            return Size(
                width,
                hm == SizeInfo::CONTENT ? 0 : height);
        }

        std::vector<int> heights(col_count_, 0);
        std::vector<int> offsets(col_count_, 0);
        std::vector<size_t> c_ids(col_count_, 0);
        std::vector<size_t> d_ids(col_count_, 0);
        auto item_count = source_->onGetListDataCount(parent_);

        columns_.setHorizontal(0, width);

        for (size_t i = 0; i < col_count_; ++i) {
            auto& r = cur_records_[i];
            if (!r.is_null) {
                r.cur_offset = (std::max)(r.cur_offset, 0);
            }
            /**
             * 使用上次保存的数据索引。
             * 可能造成顺序不稳定。
             */
            d_ids[i] = r.cur_data_pos;
            offsets[i] = r.cur_offset;
        }

        parent_->freezeLayout();

        /**
         * 一行一行遍历，当遇到排满的列时则回卷，直到所有的列
         * 均排满，或者数据耗尽。
         */
        size_t v_idx = 0;
        for (;;) {
            bool is_completed = true;
            auto cur_col_count = col_count_;
            for (size_t _col = 0; _col < cur_col_count; ++_col) {
                if (cur_col_count >= col_count_ * 2) {
                    break;
                }

                auto col = _col % col_count_;
                const auto& record = cur_records_[col];

                if (record.is_null) {
                    ++cur_col_count;
                    continue;
                }
                if (d_ids[col] >= item_count) {
                    ++cur_col_count;
                    continue;
                }
                if (heights[col] >= height + offsets[col]) {
                    ++cur_col_count;
                    continue;
                }
                is_completed = false;

                auto item = columns_[col].findAndInsertItem(
                    c_ids[col], source_->onGetListItemId(parent_, d_ids[col]));
                if (!item) {
                    item = parent_->makeNewItem(d_ids[col], v_idx);
                    columns_[col].addItem(item, c_ids[col]);
                } else {
                    parent_->setItemData(item, d_ids[col]);
                }

                int child_max_width = columns_[col].getWidth();
                auto item_size = parent_->determineItemSize(item, child_max_width);
                heights[col] += item_size.height();

                ++v_idx;
                ++c_ids[col];

                if (heights[col] < height + offsets[col]) {
                    findNext2(d_ids[col], d_ids, &d_ids[col]);
                }
            }

            if (is_completed) {
                break;
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            auto count = columns_[i].getItemCount();
            for (size_t j = c_ids[i]; j < count; ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(c_ids[i]);
        }

        parent_->unfreezeLayout();

        return Size(width, height);
    }

    int FlowListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        std::vector<int> heights(col_count_, 0);
        std::vector<int> offsets(col_count_, 0);
        std::vector<size_t> c_ids(col_count_, 0);
        std::vector<size_t> d_ids(col_count_, 0);
        auto item_count = source_->onGetListDataCount(parent_);
        auto bounds = parent_->getContentBounds();

        columns_.setVertical(bounds.y(), bounds.bottom());
        columns_.setHorizontal(bounds.x(), bounds.right());

        for (size_t i = 0; i < col_count_; ++i) {
            auto& r = cur_records_[i];
            if (!r.is_null) {
                r.cur_offset = (std::max)(r.cur_offset, 0);
            }

            if (cur) {
                d_ids[i] = r.cur_data_pos;
                offsets[i] = r.cur_offset;
            } else {
                d_ids[i] = i;
            }
        }

        parent_->freezeLayout();

        size_t v_idx = 0;
        for (;;) {
            bool is_completed = true;
            auto cur_col_count = col_count_;
            for (size_t _col = 0; _col < cur_col_count; ++_col) {
                if (cur_col_count >= col_count_ * 2) {
                    break;
                }

                auto col = _col % col_count_;
                const auto& record = cur_records_[col];
                if (record.is_null) {
                    ++cur_col_count;
                    continue;
                }
                if (d_ids[col] >= item_count) {
                    ++cur_col_count;
                    continue;
                }
                if (heights[col] >= bounds.height() + offsets[col]) {
                    ++cur_col_count;
                    continue;
                }

                is_completed = false;

                auto item = columns_[col].getItem(c_ids[col]);
                ubassert(item);

                int width = item->item_view->getDeterminedSize().width() + item->getHoriMargins();
                int height = item->item_view->getDeterminedSize().height() + item->getVertMargins();
                parent_->layoutItem(
                    item,
                    columns_[col].getLeft(), bounds.y() + heights[col] - offsets[col],
                    width, height);
                heights[col] += height;

                ++v_idx;
                ++c_ids[col];

                if (heights[col] < bounds.height() + offsets[col]) {
                    findNext2(d_ids[col], d_ids, &d_ids[col]);
                }
            }

            if (is_completed) {
                break;
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            auto count = columns_[i].getItemCount();
            for (size_t j = c_ids[i]; j < count; ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(c_ids[i]);
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

    int FlowListLayouter::onDataChangedAtPosition(size_t pos, int offset, bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        std::vector<int> heights(col_count_, 0);
        std::vector<int> offsets(col_count_, 0);
        std::vector<size_t> c_ids(col_count_, 0);
        std::vector<size_t> d_ids(col_count_, 0);
        auto item_count = source_->onGetListDataCount(parent_);
        auto bounds = parent_->getContentBounds();

        columns_.setVertical(bounds.y(), bounds.bottom());
        columns_.setHorizontal(bounds.x(), bounds.right());

        for (size_t i = 0; i < col_count_; ++i) {
            auto& r = cur_records_[i];
            if (!r.is_null) {
                r.cur_offset = (std::max)(r.cur_offset, 0);
            }

            if (cur) {
                d_ids[i] = r.cur_data_pos;
                offsets[i] = r.cur_offset;
            } else {
                d_ids[i] = i;
            }
        }

        parent_->freezeLayout();

        size_t v_idx = 0;
        for (;;) {
            bool is_completed = true;
            auto cur_col_count = col_count_;
            for (size_t _col = 0; _col < cur_col_count; ++_col) {
                if (cur_col_count >= col_count_ * 2) {
                    break;
                }

                auto col = _col % col_count_;
                const auto& record = cur_records_[col];

                if (record.is_null) {
                    ++cur_col_count;
                    continue;
                }
                if (d_ids[col] >= item_count) {
                    ++cur_col_count;
                    continue;
                }
                if (heights[col] >= bounds.height() + offsets[col]) {
                    ++cur_col_count;
                    continue;
                }

                is_completed = false;

                auto item = columns_[col].getItem(c_ids[col]);
                ubassert(item);

                int child_max_width = columns_[col].getWidth();
                auto item_size = parent_->determineItemSize(item, child_max_width);
                parent_->layoutItem(
                    item,
                    columns_[col].getLeft(), bounds.y() + heights[col] - offsets[col],
                    item_size.width(), item_size.height());
                heights[col] += item_size.height();

                ++v_idx;
                ++c_ids[col];

                if (heights[col] < bounds.height() + offsets[col]) {
                    findNext2(d_ids[col], d_ids, &d_ids[col]);
                }
            }

            if (is_completed) {
                break;
            }
        }

        for (size_t i = 0; i < col_count_; ++i) {
            auto count = columns_[i].getItemCount();
            for (size_t j = c_ids[i]; j < count; ++j) {
                parent_->recycleItem(columns_[i].getItem(j));
            }
            columns_[i].removeItems(c_ids[i]);
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

    int FlowListLayouter::onSmoothScrollToPosition(size_t pos, int offset) {
        return 0;
    }

    int FlowListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        std::vector<size_t> d_ids(col_count_, 0);
        for (size_t i = 0; i < col_count_; ++i) {
            auto f = columns_[i].getFront();
            if (f) {
                d_ids[i] = f->data_pos;
            }
        }

        parent_->freezeLayout();

        while (!columns_.isTopFilled(dy)) {
            bool is_completed = true;
            auto rem_count = col_count_;
            for (size_t _col = col_count_ * 2; _col-- > rem_count;) {
                auto col = _col % col_count_;
                if (columns_[col].isTopFilled(dy)) {
                    if (rem_count > 0) --rem_count;
                    continue;
                }
                if (!findPrev2(d_ids[col], d_ids, &d_ids[col])) {
                    if (rem_count > 0) --rem_count;
                    continue;
                }

                is_completed = false;

                int child_max_width = columns_[col].getWidth();
                auto new_item = parent_->makeNewItem(d_ids[col], 0);

                auto item_size = parent_->determineItemSize(new_item, child_max_width);
                parent_->layoutItem(
                    new_item,
                    columns_[col].getLeft(), columns_[col].getItemsTop() - item_size.height(),
                    item_size.width(), item_size.height());
                columns_[col].addItem(new_item, 0);
            }

            if (is_completed) {
                break;
            }
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

        std::vector<size_t> d_ids(col_count_, 0);
        for (size_t i = 0; i < col_count_; ++i) {
            auto f = columns_[i].getRear();
            if (f) {
                d_ids[i] = f->data_pos;
            }
        }

        parent_->freezeLayout();

        while (!columns_.isBottomFilled(dy)) {
            bool is_completed = true;
            auto cur_col_count = col_count_;
            for (size_t _col = 0; _col < cur_col_count; ++_col) {
                if (cur_col_count >= col_count_ * 2) {
                    break;
                }

                auto col = _col % col_count_;
                if (columns_[col].isBottomFilled(dy)) {
                    ++cur_col_count;
                    continue;
                }
                if (!findNext2(d_ids[col], d_ids, &d_ids[col])) {
                    ++cur_col_count;
                    continue;
                }
                if (d_ids[col] >= source_->onGetListDataCount(parent_)) {
                    ++cur_col_count;
                    continue;
                }

                is_completed = false;

                int child_max_width = columns_[col].getWidth();
                auto new_item = parent_->makeNewItem(d_ids[col], parent_->getChildCount());

                auto item_size = parent_->determineItemSize(new_item, child_max_width);
                parent_->layoutItem(
                    new_item,
                    columns_[col].getLeft(), columns_[col].getItemsBottom(),
                    item_size.width(), item_size.height());
                columns_[col].addItem(new_item);
            }

            if (is_completed) {
                break;
            }
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
        columns_.clear();
    }

    void FlowListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *next = *prev = 0;
            return;
        }

        auto data_count = source_->onGetListDataCount(parent_);
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
                avg_height += item->getHeight();
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
            child_height = int(max_col_height / max_col_count);
        }

        auto cur_row = cur_records_[max_col].cur_data_pos / col_count_;
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
                record.cur_data_pos = item->data_pos;
                record.cur_offset = bounds.y() - item->getY();
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

    size_t FlowListLayouter::getCurrentDataPos() const {
        size_t pos = (std::numeric_limits<size_t>::max)();
        for (const auto& r : cur_records_) {
            if (r.is_null) {
                continue;
            }
            pos = (std::min)(pos, r.cur_data_pos);
            if (pos == 0) {
                break;
            }
        }
        return pos;
    }

    bool FlowListLayouter::canScrollToTop() const {
        auto item_count = source_->onGetListDataCount(parent_);
        return !(columns_.isAllAtTop() && columns_.isAllAtCeil(item_count));
    }

    bool FlowListLayouter::canScrollToBottom() const {
        auto item_count = source_->onGetListDataCount(parent_);
        return !(columns_.isAllAtBottom() && columns_.isAllAtFloor(item_count));
    }

    bool FlowListLayouter::canScrollToLeft() const {
        return false;
    }

    bool FlowListLayouter::canScrollToRight() const {
        return false;
    }

}
