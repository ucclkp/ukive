// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TREE_TREE_NODE_H_
#define UKIVE_VIEWS_TREE_TREE_NODE_H_

#include <string>


namespace ukive {

    class TreeNode {
    public:
        TreeNode() = default;

        void addChildCount(int inc);

        int image_rid;
        std::u16string text;

        int level = 0;
        int child_count = 0;
        bool is_expand = false;
        bool is_selected = false;
        TreeNode* parent = nullptr;
    };

}

#endif  // UKIVE_VIEWS_TREE_TREE_NODE_H_