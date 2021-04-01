// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/views/tree/tree_node.h"

#include "utils/log.h"


namespace ukive {

    TreeNode::TreeNode() {}

    TreeNode::~TreeNode() {
        utl::STLDeleteElements(&children_);
    }

    void TreeNode::setNodeId(int id) {
        node_id_ = id;
    }

    void TreeNode::setExpanded(bool expanded) {
        if (expanded == is_expanded_) {
            return;
        }

        is_expanded_ = expanded;

        if (is_expanded_) {
            DCHECK(edc_ == 0);
            for (auto node : children_) {
                edc_ += node->edc_ + 1;
            }
            if (parent_) {
                parent_->addEDC(edc_);
            }
        } else {
            if (parent_) {
                parent_->subEDC(edc_);
            }
            edc_ = 0;
        }
    }

    void TreeNode::addNode(TreeNode* node) {
        addNode(children_.size(), node);
    }

    void TreeNode::addNode(size_t index, TreeNode* node) {
        if (!node) {
            return;
        }
        if (node->parent_) {
            return;
        }
        if (index > children_.size()) {
            index = children_.size();
        }

        node->parent_ = this;
        children_.insert(children_.begin() + index, node);
        addEDC(node->edc_ + 1);
        node->addLevel(level_ + 1);
    }

    void TreeNode::removeNode(TreeNode* node, bool del) {
        if (!node) {
            return;
        }

        for (auto it = children_.begin(); it != children_.end(); ++it) {
            if (*it == node) {
                subEDC(node->edc_ + 1);
                if (del) {
                    delete node;
                } else {
                    node->parent_ = nullptr;
                    node->subLevel(level_ + 1);
                }
                children_.erase(it);
                return;
            }
        }
    }

    void TreeNode::removeNode(size_t index, bool del) {
        if (index >= children_.size()) {
            return;
        }

        auto node = children_[index];
        subEDC(node->edc_ + 1);
        if (del) {
            delete node;
        } else {
            node->parent_ = nullptr;
            node->subLevel(level_ + 1);
        }
        children_.erase(children_.begin() + index);
    }

    void TreeNode::removeAllNodes(bool del) {
        size_t edc = children_.size();
        for (auto node : children_) {
            edc += node->edc_;
            if (del) {
                delete node;
            } else {
                node->parent_ = nullptr;
                node->subLevel(level_ + 1);
            }
        }
        children_.clear();
        subEDC(edc);
    }

    bool TreeNode::isExpanded() const {
        return is_expanded_;
    }

    int TreeNode::getNodeId() const {
        return node_id_;
    }

    size_t TreeNode::getLevel() const {
        return level_;
    }

    TreeNode* TreeNode::getParent() const {
        return parent_;
    }

    size_t TreeNode::getChildCount() const {
        return children_.size();
    }

    TreeNode* TreeNode::getChildAt(size_t index) const {
        if (index >= children_.size()) {
            return nullptr;
        }
        return children_[index];
    }

    TreeNode* TreeNode::getExpandedDescendantAt(size_t pos) const {
        return getExpandedDescendantAt(pos, 0);
    }

    size_t TreeNode::getExpandedDescendantCount() const {
        return edc_;
    }

    void TreeNode::addEDC(size_t inc) {
        if (!is_expanded_) {
            return;
        }

        edc_ += inc;
        if (parent_) {
            parent_->addEDC(inc);
        }
    }

    void TreeNode::subEDC(size_t dec) {
        if (!is_expanded_) {
            return;
        }

        DCHECK(edc_ >= dec + children_.size());

        edc_ -= dec;
        if (parent_) {
            parent_->subEDC(dec);
        }
    }

    void TreeNode::addLevel(size_t inc) {
        level_ += inc;
        for (auto node : children_) {
            node->addLevel(inc);
        }
    }

    void TreeNode::subLevel(size_t dec) {
        DCHECK(level_ >= dec);

        level_ -= dec;
        for (auto node : children_) {
            node->subLevel(dec);
        }
    }

    TreeNode* TreeNode::getExpandedDescendantAt(size_t pos, size_t cur) const {
        for (auto node : children_) {
            if (pos == cur) {
                return node;
            }

            ++cur;

            if (!node->is_expanded_) {
                continue;
            }

            if (pos < cur + node->edc_) {
                auto target = node->getExpandedDescendantAt(pos, cur);
                DCHECK(target);
                return target;
            }

            cur += node->edc_;
        }
        return nullptr;
    }


}
