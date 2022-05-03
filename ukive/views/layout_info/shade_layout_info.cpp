// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "shade_layout_info.h"

#include "ukive/views/view.h"


namespace ukive {

    // ShadeViewRect
    ShadeViewRect::ShadeViewRect() {}

    bool ShadeViewRect::operator==(const ShadeViewRect& rhs) const {
        return equal(rhs);
    }
    bool ShadeViewRect::operator!=(const ShadeViewRect& rhs) const {
        return !equal(rhs);
    }

    bool ShadeViewRect::equal(const ShadeViewRect& rhs) const {
        if (view != rhs.view) {
            return false;
        }
        if (!view) {
            if (rect != rhs.rect) {
                return false;
            }
        }
        return true;
    }

    void ShadeViewRect::set(const Rect& rect) {
        this->rect = rect;
    }

    void ShadeViewRect::set(View* v) {
        view = v;
    }

    const Rect& ShadeViewRect::get() {
        if (!view) {
            return rect;
        }
        rect = view->getContentBoundsInRoot();
        return rect;
    }

    bool ShadeViewRect::valid() const {
        if (view) {
            return true;
        }
        if (!rect.empty()) {
            return true;
        }
        return false;
    }


    // ShadeParams
    ShadeParams::ShadeParams() {
    }

    bool ShadeParams::operator==(const ShadeParams& rhs) const {
        return equal(rhs);
    }
    bool ShadeParams::operator!=(const ShadeParams& rhs) const {
        return !equal(rhs);
    }

    bool ShadeParams::equal(const ShadeParams& rhs) const {
        if (type != rhs.type ||
            gravity != rhs.gravity ||
            is_max_visible != rhs.is_max_visible)
        {
            return false;
        }

        if (type == LT_POS) {
            if (limit != rhs.limit ||
                relative != rhs.relative)
            {
                return false;
            }
            if (pos != rhs.pos ||
                pos_padding != rhs.pos_padding ||
                is_evaded != rhs.is_evaded)
            {
                return false;
            }
        } else {
            if (anchor != rhs.anchor) {
                return false;
            }
            if (is_max_visible && is_discretized != rhs.is_discretized) {
                return false;
            }
        }

        return true;
    }


    // ShadeLayoutInfo
    ShadeLayoutInfo::ShadeLayoutInfo() {
    }

}
