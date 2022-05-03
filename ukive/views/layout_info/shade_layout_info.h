// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_INFO_SHADE_LAYOUT_INFO_H_
#define UKIVE_VIEWS_LAYOUT_INFO_SHADE_LAYOUT_INFO_H_

#include "ukive/graphics/rect.hpp"
#include "ukive/views/layout_info/gravity.h"
#include "ukive/views/layout_info/layout_info.h"
#include "ukive/views/view_ref.h"


namespace ukive {

    class View;

    class ShadeViewRect {
    public:
        ShadeViewRect();

        bool operator==(const ShadeViewRect& rhs) const;
        bool operator!=(const ShadeViewRect& rhs) const;

        bool equal(const ShadeViewRect& rhs) const;

        void set(const Rect& rect);
        void set(View* v);
        const Rect& get();

        bool valid() const;

    private:
        Rect rect;
        ViewRef view;
    };


    class ShadeParams {
    public:
        enum LayoutType {
            LT_SNAP,
            LT_POS,
        };

        ShadeParams();

        bool operator==(const ShadeParams& rhs) const;
        bool operator!=(const ShadeParams& rhs) const;

        bool equal(const ShadeParams& rhs) const;

        // 公用
        LayoutType type = LT_POS;
        int gravity = GV_NONE;
        bool is_max_visible = true;

        // 贴靠
        ShadeViewRect anchor;
        bool is_discretized = true;

        // 指定位置
        Point pos{};
        Padding pos_padding;
        ViewRef relative;
        ShadeViewRect limit;
        bool is_evaded = false;
    };


    class ShadeLayoutInfo : public LayoutInfo {
    public:
        ShadeLayoutInfo();

        ShadeParams params;
        int adj_corner = GV_NONE;
        int adj_gravity = GV_NONE;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_INFO_SHADE_LAYOUT_INFO_H_