// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "container_layout.h"


namespace vevah {

    ContainerLayout::ContainerLayout(ukive::Context c)
        : ContainerLayout(c, {}) {}

    ContainerLayout::ContainerLayout(ukive::Context c, ukive::AttrsRef attrs)
        : super(c, attrs) {}

    ukive::Size ContainerLayout::onDetermineSize(const ukive::SizeInfo& info) {
        return super::onDetermineSize(info);
    }

    void ContainerLayout::onLayout(
        const ukive::Rect& new_bounds,
        const ukive::Rect& old_bounds)
    {

    }

}
