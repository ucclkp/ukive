// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_RENDER_NODE_RENDER_NODE_H_
#define UKIVE_GRAPHICS_RENDER_NODE_RENDER_NODE_H_

#include <vector>


namespace ukive {

    class RenderNode {
    public:
        RenderNode();
        ~RenderNode() = default;

        void setParent(RenderNode* node);
        void addChild(RenderNode* node);
        void removeChild(RenderNode* node);
        void removeAllChild();

    private:
        RenderNode* parent_;
        std::vector<RenderNode*> children_;
    };
}

#endif  // UKIVE_GRAPHICS_RENDER_NODE_RENDER_NODE_H_