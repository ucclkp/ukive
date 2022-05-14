// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef VEVAH_CONTAINER_LAYOUT_H_
#define VEVAH_CONTAINER_LAYOUT_H_

#include "ukive/views/layout/layout_view.h"


namespace vevah {

    class ContainerLayout :
        public ukive::LayoutView
    {
    public:
        explicit ContainerLayout(ukive::Context c);
        ContainerLayout(ukive::Context c, ukive::AttrsRef attrs);

    protected:
        ukive::Size onDetermineSize(const ukive::SizeInfo& info) override;
        void onLayout(
            const ukive::Rect& new_bounds,
            const ukive::Rect& old_bounds) override;

    private:
        using super = LayoutView;
    };

}

#endif  // VEVAH_CONTAINER_LAYOUT_H_