// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_NON_CLIENT_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_NON_CLIENT_LAYOUT_H_

#include "ukive/views/layout/layout_view.h"
#include "ukive/graphics/padding.hpp"


namespace ukive {

    class NonClientLayout : public LayoutView {
    public:
        explicit NonClientLayout(Context c);
        NonClientLayout(Context c, AttrsRef attrs);

        virtual HitPoint onNCHitTest(int x, int y);

        void setNonClientPadding(int left, int top, int right, int bottom);
        void setSizeHandlePadding(int left, int top, int right, int bottom);

        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;
        void onDraw(Canvas* canvas) override;

        void onAttachedToWindow(Window* w) override;

    private:
        Padding nc_padding_;
        Padding sh_padding_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_NON_CLIENT_LAYOUT_H_