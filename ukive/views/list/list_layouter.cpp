// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/list/list_layouter.h"

namespace ukive {

    ListLayouter::ListLayouter() {}

    void ListLayouter::bind(ListView* parent, ListSource* source) {
        parent_ = parent;
        source_ = source;
    }

    bool ListLayouter::isAvailable() const {
        return parent_ && source_;
    }

}