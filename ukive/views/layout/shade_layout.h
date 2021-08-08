// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_SHADE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_SHADE_LAYOUT_H_

#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/layout_info/shade_layout_info.h"


namespace ukive {

    class ShadeLayout : public SimpleLayout {
    public:
        explicit ShadeLayout(Context c);
        ShadeLayout(Context c, AttrsRef attrs);

        LayoutInfo* makeExtraLayoutInfo() const override;
        LayoutInfo* makeExtraLayoutInfo(AttrsRef attrs) const override;
        bool isValidExtraLayoutInfo(LayoutInfo* lp) const override;

        void onLayout(
            const Rect& new_bounds,
            const Rect& old_bounds) override;

    private:
        using super = SimpleLayout;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_SHADE_LAYOUT_H_