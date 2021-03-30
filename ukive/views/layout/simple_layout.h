// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_LAYOUT_SIMPLE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_SIMPLE_LAYOUT_H_

#include "ukive/views/layout/layout_view.h"


namespace ukive {

    class SimpleLayout : public LayoutView {
    public:
        explicit SimpleLayout(Context c);
        SimpleLayout(Context c, AttrsRef attrs);

        Size onDetermineSize(const SizeInfo& info) override;
        void onLayout(
            const Rect& new_bounds, const Rect& old_bounds) override;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_SIMPLE_LAYOUT_H_