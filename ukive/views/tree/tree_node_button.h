// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TREE_TREE_NODE_BUTTON_H_
#define UKIVE_VIEWS_TREE_TREE_NODE_BUTTON_H_

#include "ukive/views/view.h"


namespace ukive {

    class Path;

    class TreeNodeButton : public View {
    public:
        enum Status {
            NONE,
            COLLAPSED,
            EXPANDED,
        };

        explicit TreeNodeButton(Context c);
        TreeNodeButton(Context c, AttrsRef attrs);

        void setStatus(Status s);

        // View
        Size onDetermineSize(const SizeInfo& info) override;
        void onDraw(Canvas* canvas) override;

    private:
        int size_ = 0;
        int tri_length_ = 0;
        int tri_height_ = 0;
        Status status_ = NONE;
        std::unique_ptr<Path> tri_path_;
    };

}

#endif  // UKIVE_VIEWS_TREE_TREE_NODE_BUTTON_H_