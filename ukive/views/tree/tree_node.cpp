// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/tree/tree_node.h"


namespace ukive {

    void TreeNode::addChildCount(int inc) {
        child_count += inc;
        if (parent) {
            parent->addChildCount(inc);
        }
    }

}
