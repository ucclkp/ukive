// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LIST_LIST_ITEM_INTERACT_HELPER_H_
#define UKIVE_VIEWS_LIST_LIST_ITEM_INTERACT_HELPER_H_

#include <vector>


namespace ukive {

    class ListItemInteractHelper {
    public:
        ListItemInteractHelper();
        virtual ~ListItemInteractHelper() = default;

        virtual void onItemHelperSelectItem(bool selected, size_t index) {}

        void selectItem(size_t index);
        void deselectItem(size_t index);
        void deselectAllItems();
        void setMultiSelectionEnabled(bool enabled);

        void notifyHelperItemAdded(size_t index);
        void notifyHelperItemAdded(size_t start, size_t count);
        void notifyHelperItemRemoved(size_t index);
        void notifyHelperItemRemoved(size_t start, size_t count);

        bool isMultiSelectionEnabled() const;
        bool isItemSelected(size_t index) const;
        const std::vector<size_t>& getSelectedIndices() const;

    private:
        void insertIndex(size_t index);

        bool multi_sel_enabled_ = false;
        std::vector<size_t> sel_indices_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_ITEM_INTERACT_HELPER_H_