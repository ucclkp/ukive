// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "qtree_node.h"


namespace shell {

    QTreeNode::QTreeNode() {
        level = 0;
        indexX = -1;
        indexY = -1;
        rough = 0.f;

        child[0] = nullptr;
        child[1] = nullptr;
        child[2] = nullptr;
        child[3] = nullptr;

        next = nullptr;
    }

    QTreeNode::~QTreeNode() {
    }

}