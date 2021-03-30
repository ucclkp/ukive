// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "render_node.h"


namespace ukive {

    RenderNode::RenderNode()
        :parent_(nullptr) {
    }


    void RenderNode::setParent(RenderNode* node) {
        parent_ = node;
    }

    void RenderNode::addChild(RenderNode* node) {
        children_.push_back(node);
    }

    void RenderNode::removeChild(RenderNode* node) {

    }

    void RenderNode::removeAllChild() {
        children_.clear();
    }

}