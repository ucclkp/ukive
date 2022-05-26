// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "container_layout.h"

#include "ukive/graphics/canvas.h"


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

    bool ContainerLayout::onInputEvent(ukive::InputEvent* e) {
        bool consumed = super::onInputEvent(e);

        switch (e->getEvent()) {
        case ukive::InputEvent::EV_HAUL:
            is_haul_in_ = true;
            requestDraw();
            consumed = true;
            break;

        case ukive::InputEvent::EV_HAUL_END:
            is_haul_in_ = false;
            requestDraw();
            consumed = true;
            break;

        case ukive::InputEvent::EV_HAUL_LEAVE:
            is_haul_in_ = false;
            requestDraw();
            consumed = true;
            break;

        default:
            break;
        }

        return consumed;
    }

    void ContainerLayout::onDrawOverChildren(ukive::Canvas* canvas) {
        super::onDrawOverChildren(canvas);

        if (is_haul_in_) {
            canvas->fillRect(
                ukive::RectF(getContentBounds()), ukive::Color::Grey500);
        }
    }

}
