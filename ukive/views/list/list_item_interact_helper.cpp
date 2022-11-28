// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_item_interact_helper.h"

#include <algorithm>


namespace ukive {

    ListItemInteractHelper::ListItemInteractHelper() {}

    void ListItemInteractHelper::selectItem(size_t index) {
        auto it = std::find(sel_indices_.begin(), sel_indices_.end(), index);
        if (it != sel_indices_.end()) {
            return;
        }

        if (multi_sel_enabled_) {
            insertIndex(index);
            onItemHelperSelectItem(true, index);
        } else {
            if (sel_indices_.empty()) {
                insertIndex(index);
                onItemHelperSelectItem(true, index);
            } else {
                auto& prev = sel_indices_[0];
                onItemHelperSelectItem(false, prev);
                prev = index;
                onItemHelperSelectItem(true, index);
            }
        }
    }

    void ListItemInteractHelper::deselectItem(size_t index) {
        auto it = std::find(sel_indices_.begin(), sel_indices_.end(), index);
        if (it == sel_indices_.end()) {
            return;
        }

        sel_indices_.erase(it);
        onItemHelperSelectItem(false, index);
    }

    void ListItemInteractHelper::deselectAllItems() {
        for (auto s : sel_indices_) {
            onItemHelperSelectItem(false, s);
        }
        sel_indices_.clear();
    }

    void ListItemInteractHelper::setMultiSelectionEnabled(bool enabled) {
        multi_sel_enabled_ = enabled;
    }

    void ListItemInteractHelper::notifyHelperItemAdded(size_t index) {
        notifyHelperItemAdded(index, 1u);
    }

    void ListItemInteractHelper::notifyHelperItemAdded(size_t start, size_t count) {
        if (!count) return;
        for (auto& s : sel_indices_) {
            if (s >= start) {
                s += count;
            }
        }
    }

    void ListItemInteractHelper::notifyHelperItemRemoved(size_t index) {
        notifyHelperItemRemoved(index, 1u);
    }

    void ListItemInteractHelper::notifyHelperItemRemoved(size_t start, size_t count) {
        if (!count) return;
        for (auto it = sel_indices_.begin(); it != sel_indices_.end();) {
            if (*it >= start && *it < start + count) {
                it = sel_indices_.erase(it);
            } else {
                if (*it >= start + count) {
                    *it -= count;
                }
                ++it;
            }
        }
    }

    bool ListItemInteractHelper::isMultiSelectionEnabled() const {
        return multi_sel_enabled_;
    }

    bool ListItemInteractHelper::isItemSelected(size_t index) const {
        return std::find(
            sel_indices_.cbegin(),
            sel_indices_.cend(),
            index) != sel_indices_.cend();
    }

    const std::vector<size_t>& ListItemInteractHelper::getSelectedIndices() const {
        return sel_indices_;
    }

    void ListItemInteractHelper::insertIndex(size_t index) {
        sel_indices_.insert(
            std::upper_bound(
                sel_indices_.begin(), sel_indices_.end(), index),
            index);
    }

}
