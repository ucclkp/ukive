// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "list_source.h"


namespace ukive {

    ListSource::ListSource()
        :notifier_(nullptr) {}

    void ListSource::notifyDataChanged() {
        if (notifier_) {
            notifier_->onDataChanged();
        }
    }

    void ListSource::notifyItemChanged(size_t start_pos, size_t count) {
        if (notifier_) {
            notifier_->onItemChanged(start_pos, count);
        }
    }

    void ListSource::notifyItemInserted(size_t start_pos, size_t count) {
        if (notifier_) {
            notifier_->onItemInserted(start_pos, count);
        }
    }

    void ListSource::notifyItemRemoved(size_t start_pos, size_t count) {
        if (notifier_) {
            notifier_->onItemRemoved(start_pos, count);
        }
    }

}