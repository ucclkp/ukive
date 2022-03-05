// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef SHELL_LOD_QTREE_NODE_H_
#define SHELL_LOD_QTREE_NODE_H_

#include "utils/math/algebra/point.hpp"


namespace shell {

    class QTreeNode {
    public:
        QTreeNode();
        ~QTreeNode();

        int level;
        int indexX;
        int indexY;
        float rough;
        utl::pt3f mincoord;
        utl::pt3f maxcoord;
        QTreeNode* child[4];

        QTreeNode* next;
    };

}

#endif  // SHELL_LOD_QTREE_NODE_H_