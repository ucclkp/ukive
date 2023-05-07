// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TREE_TREE_NODE_H_
#define UKIVE_VIEWS_TREE_TREE_NODE_H_

#include <vector>

#include "utils/stl_utils.h"


namespace ukive {

    class TreeNode {
    public:
        TreeNode();
        ~TreeNode();

        void setNodeId(int id);
        void setExpanded(bool expanded);

        void addNode(TreeNode* node);
        void addNode(size_t index, TreeNode* node);
        void removeNode(TreeNode* node, bool del = true);
        void removeNode(size_t index, bool del = true);
        void removeAllNodes(bool del = true);

        bool isExpanded() const;

        int getNodeId() const;
        size_t getLevel() const;
        TreeNode* getParent() const;
        size_t getChildCount() const;
        TreeNode* getChildAt(size_t index) const;
        TreeNode* getExpandedDescendantAt(size_t pos) const;
        size_t getExpandedDescendantCount() const;

        STL_VECTOR_ALL_ITERATORS(TreeNode*, children_);

    private:
        void addEDC(size_t inc);
        void subEDC(size_t dec);
        void addLevel(size_t inc);
        void subLevel(size_t dec);
        TreeNode* getExpandedDescendantAt(size_t pos, size_t cur) const;

        size_t edc_ = 0;
        size_t level_ = 0;

        int node_id_ = -1;
        bool is_expanded_ = false;
        TreeNode* parent_ = nullptr;

        std::vector<TreeNode*> children_;
    };

}

#endif  // UKIVE_VIEWS_TREE_TREE_NODE_H_