// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_DIRTY_REGION_H_
#define UKIVE_GRAPHICS_DIRTY_REGION_H_

#include "ukive/graphics/rect.hpp"


namespace ukive {

    class DirtyRegion {
    public:
        DirtyRegion();

        void add(const Rect& rect);
        void setOne(const Rect& rect);
        void clear();

        bool empty() const;

        Rect rect0;
        Rect rect1;
    };

}

#endif  // UKIVE_GRAPHICS_DIRTY_REGION_H_