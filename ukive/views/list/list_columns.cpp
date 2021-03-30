// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/list/list_columns.h"

#include <algorithm>

#include "utils/log.h"
#include "utils/number.hpp"

#include "ukive/views/list/list_item.h"


namespace ukive {

    // Column
    Column::Column()
        : top_(0),
          bottom_(0),
          left_(0),
          right_(0) {
    }

    void Column::setVertical(int top, int bottom) {
        top_ = top;
        bottom_ = bottom;
    }

    void Column::setHorizontal(int left, int right) {
        left_ = left;
        right_ = right;
    }

    void Column::addItem(ListItem* item) {
        items_.push_back(item);
    }

    void Column::addItem(ListItem* item, int index) {
        items_.insert(items_.begin() + index, item);
    }

    void Column::removeItem(int index) {
        items_.erase(items_.begin() + index);
    }

    void Column::removeItems(int start) {
        items_.erase(items_.begin() + start, items_.end());
    }

    void Column::removeItems(int start, int length) {
        int end = start + length;
        items_.erase(items_.begin() + start, items_.begin() + end);
    }

    void Column::clear() {
        items_.clear();
    }

    int Column::getLeft() const {
        return left_;
    }

    int Column::getRight() const {
        return right_;
    }

    int Column::getWidth() const {
        return right_ - left_;
    }

    int Column::getItemsTop() const {
        if (!items_.empty()) {
            return items_.front()->getMgdTop();
        }
        return top_;
    }

    int Column::getItemsBottom() const {
        if (!items_.empty()) {
            return items_.back()->getMgdBottom();
        }
        return top_;
    }

    int Column::getItemCount() const {
        return utl::num_cast<int>(items_.size());
    }

    int Column::getFinalScroll(int dy) const {
        if (dy > 0) {
            if (top_ - getItemsTop() < dy) {
                dy = std::max(top_ - getItemsTop(), 0);
            }
        } else if (dy < 0) {
            if (bottom_ - getItemsBottom() > dy) {
                dy = std::min(bottom_ - getItemsBottom(), 0);
            }
        }
        return dy;
    }

    int Column::getIndexOfItem(int data_pos) const {
        int index = 0;
        for (auto it = items_.cbegin(); it != items_.cend(); ++it, ++index) {
            if ((*it)->data_pos == data_pos) {
                return index;
            }
            if ((*it)->data_pos > data_pos) {
                break;
            }
        }
        return -1;
    }

    int Column::getIndexOfFirstVisible(int dy) const {
        int index = 0;
        for (auto it = items_.cbegin(); it != items_.cend(); ++it, ++index) {
            if ((*it)->getMgdBottom() + dy > top_) {
                return index;
            }
        }
        return -1;
    }

    int Column::getIndexOfLastVisible(int dy) const {
        int index = getItemCount() - 1;
        for (auto it = items_.crbegin(); it != items_.crend(); ++it, --index) {
            if ((*it)->getMgdTop() + dy < bottom_) {
                return index;
            }
        }
        return -1;
    }

    ListItem* Column::getFront() const {
        if (!items_.empty()) {
            return items_.front();
        }
        return nullptr;
    }

    ListItem* Column::getRear() const {
        if (!items_.empty()) {
            return items_.back();
        }
        return nullptr;
    }

    ListItem* Column::getItem(int index) const {
        if (index >= 0 && index < utl::num_cast<int>(items_.size())) {
            return items_[index];
        }
        return nullptr;
    }

    ListItem* Column::getFirstVisible(int dy) const {
        for (auto it = items_.cbegin(); it != items_.cend(); ++it) {
            if ((*it)->getMgdBottom() + dy > top_) {
                return *it;
            }
        }
        return nullptr;
    }

    ListItem* Column::getLastVisible(int dy) const {
        for (auto it = items_.crbegin(); it != items_.crend(); ++it) {
            if ((*it)->getMgdTop() + dy < bottom_) {
                return *it;
            }
        }
        return nullptr;
    }

    ListItem* Column::getItemByPos(int data_pos) const {
        for (auto it = items_.cbegin(); it != items_.cend(); ++it) {
            if ((*it)->data_pos == data_pos) {
                return *it;
            }
            if ((*it)->data_pos > data_pos) {
                break;
            }
        }
        return nullptr;
    }

    ListItem* Column::findAndInsertItem(int start_index, int item_id) {
        if (start_index < 0 || start_index >= utl::num_cast<int>(items_.size())) {
            return nullptr;
        }

        auto item = items_[start_index];
        if (item->item_id == item_id) {
            return item;
        }

        ++start_index;
        ListItem* target = nullptr;
        for (auto it = items_.begin() + start_index; it != items_.end(); ++it) {
            if ((*it)->item_id == item_id) {
                target = *it;
                items_.erase(it);
                break;
            }
        }

        if (target) {
            addItem(target, start_index);
        }
        return target;
    }

    ListItem* Column::findItemFromView(View* v) const {
        for (const auto& item : items_) {
            if (item->item_view == v) {
                return item;
            }
        }
        return nullptr;
    }

    bool Column::atTop() const {
        if (items_.empty() || items_.front()->getMgdTop() == top_) {
            return true;
        }
        return false;
    }

    bool Column::atBottom() const {
        if (items_.empty() || items_.back()->getMgdBottom() == bottom_) {
            return true;
        }
        return false;
    }

    bool Column::isTopFilled(int dy) const {
        if (!items_.empty()) {
            if (items_.front()->getMgdTop() + dy <= top_) {
                return true;
            }
        }
        return false;
    }

    bool Column::isBottomFilled(int dy) const {
        if (!items_.empty()) {
            if (items_.back()->getMgdBottom() + dy >= bottom_) {
                return true;
            }
        }
        return false;
    }


    // ColumnCollection
    ColumnCollection::ColumnCollection(int col_count)
        : col_count_(col_count) {
          columns_.assign(col_count, {});
    }

    Column& ColumnCollection::operator[](int col) {
        return columns_[col];
    }

    const Column& ColumnCollection::operator[](int col) const {
        return columns_[col];
    }

    void ColumnCollection::setVertical(int top, int bottom) {
        for (auto& col : columns_) {
            col.setVertical(top, bottom);
        }
    }

    void ColumnCollection::setHorizontal(int left, int right) {
        int cur_left = left;
        int child_width = (right - left) / static_cast<float>(col_count_);
        int remained_width = (right - left) - col_count_ * child_width;
        for (int i = 0; i < col_count_; ++i) {
            if (remained_width > 0) {
                columns_[i].setHorizontal(cur_left, cur_left + child_width + 1);
                cur_left += child_width + 1;
                --remained_width;
            } else {
                columns_[i].setHorizontal(cur_left, cur_left + child_width);
                cur_left += child_width;
            }
        }
    }

    void ColumnCollection::clear() {
        for (auto& col : columns_) {
            col.clear();
        }
    }

    int ColumnCollection::getFinalScroll(int dy) const {
        int final_dy = 0;
        for (auto& col : columns_) {
            auto tmp_dy = col.getFinalScroll(dy);
            if (std::abs(tmp_dy) > std::abs(final_dy)) {
                final_dy = tmp_dy;
            }
        }
        return final_dy;
    }

    ListItem* ColumnCollection::getFirst() const {
        return columns_[0].getFront();
    }

    ListItem* ColumnCollection::getLast() const {
        int count = columns_[0].getItemCount();
        if (count <= 0) {
            return nullptr;
        }
        for (int i = col_count_ - 1; i >= 0; --i) {
            auto item = columns_[i].getItem(count - 1);
            if (item) {
                return item;
            }
        }
        return nullptr;
    }

    ListItem* ColumnCollection::getTopStart() const {
        int index = 0;
        int max_data_pos = 0;
        ListItem* start_item = nullptr;
        for (const auto& col : columns_) {
            auto front = col.getFront();
            if (front) {
                if (!start_item) {
                    max_data_pos = front->data_pos - index;
                    start_item = front;
                } else {
                    if (front->data_pos - index > max_data_pos) {
                        max_data_pos = front->data_pos - index;
                        start_item = front;
                    }
                }
            }
            ++index;
        }
        return start_item;
    }

    ListItem* ColumnCollection::getBottomStart() const {
        int index = col_count_ - 1;
        int min_data_pos = 0;
        ListItem* start_item = nullptr;
        for (auto it = columns_.crbegin(); it != columns_.crend(); ++it) {
            auto rear = (*it).getRear();
            if (rear) {
                if (!start_item) {
                    min_data_pos = rear->data_pos - index;
                    start_item = rear;
                } else {
                    if (rear->data_pos - index < min_data_pos) {
                        min_data_pos = rear->data_pos - index;
                        start_item = rear;
                    }
                }
            }
            --index;
        }
        return start_item;
    }

    ListItem* ColumnCollection::getItemByPos(int data_pos) const {
        //int row = data_pos / columns_.size();
        int col = data_pos % columns_.size();
        return columns_[col].getItemByPos(data_pos);
    }

    ListItem* ColumnCollection::getTopmost() const {
        int top = 0;
        ListItem* result = nullptr;
        for (const auto& c : columns_) {
            auto front = c.getFront();
            if (!front) {
                continue;
            }

            if (!result || front->getMgdTop() < top) {
                top = front->getMgdTop();
                result = front;
            }
        }
        return result;
    }

    ListItem* ColumnCollection::getBottomost() const {
        int bottom = 0;
        ListItem* result = nullptr;
        for (const auto& c : columns_) {
            auto rear = c.getRear();
            if (!rear) {
                continue;
            }

            if (!result || rear->getMgdBottom() > bottom) {
                bottom = rear->getMgdBottom();
                result = rear;
            }
        }
        return result;
    }

    ListItem* ColumnCollection::findItemFromView(View* v) const {
        for (const auto& c : columns_) {
            auto item = c.findItemFromView(v);
            if (item) {
                return item;
            }
        }
        return nullptr;
    }

    bool ColumnCollection::isAllAtTop() const {
        for (const auto& c : columns_) {
            if (!c.atTop()) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isAllAtBottom() const {
        for (const auto& c : columns_) {
            if (!c.atBottom()) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isTopFilled(int dy) const {
        for (const auto& c : columns_) {
            if (!c.isTopFilled(dy)) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isBottomFilled(int dy) const {
        for (const auto& c : columns_) {
            if (!c.isBottomFilled(dy)) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isBottomOneFilled(int dy) const {
        for (const auto& c : columns_) {
            if (c.isBottomFilled(dy)) {
                return true;
            }
        }
        return false;
    }

    bool ColumnCollection::isAllAtCeil(int item_count) const {
        for (const auto& c : columns_) {
            auto front = c.getFront();
            if (front) {
                if (front->data_pos >= col_count_) {
                    return false;
                }
            }
        }
        return true;
    }

    bool ColumnCollection::isAllAtFloor(int item_count) const {
        for (const auto& c : columns_) {
            auto rear = c.getRear();
            if (rear) {
                if (rear->data_pos + col_count_ < item_count) {
                    return false;
                }
            }
        }
        return true;
    }

}