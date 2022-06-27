// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "container_layout.h"

#include "ukive/graphics/canvas.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/window/haul_source.h"

#include "vevah/sequence_layout_delegate.h"


namespace {

    bool isLayouter(const std::string_view& name) {
        return name == "TitleBarLayout" ||
            name == "SimpleLayout" ||
            name == "SequenceLayout" ||
            name == "ListView" ||
            name == "RestraintLayout" ||
            name == "ScrollView" ||
            name == "TabView";
    }

    ukive::ViewDelegate* obtainViewDelegate(const std::string_view& name) {
        if (name == "SequenceLayout") {
            return new vevah::SequenceLayoutDelegate();
        }

        return nullptr;
    }

}

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
        super::onLayout(new_bounds, old_bounds);
    }

    void ContainerLayout::onDrawOverChildren(ukive::Canvas* canvas) {
        super::onDrawOverChildren(canvas);

        using namespace ukive;

        if (is_haul_in_) {
            canvas->fillRect(RectF(getContentBounds()), Color::Green100);
        }

        auto c = getContext();

        if (hasChildren() && is_view_selected_) {
            auto bounds = getChildAt(0)->getBounds();
            canvas->drawRect(RectF(bounds), c.dp2px(3), Color::Blue500);
        }
    }

    bool ContainerLayout::onHookInputEvent(ukive::InputEvent* e) {
        if (e->getEvent() == ukive::InputEvent::EV_HAUL ||
            e->getEvent() == ukive::InputEvent::EV_HAUL_END ||
            e->getEvent() == ukive::InputEvent::EV_HAUL_LEAVE)
        {
            return false;
        }
        return mode_ != Mode::Layout;
    }

    bool ContainerLayout::onInputEvent(ukive::InputEvent* e) {
        using namespace ukive;
        bool consumed = super::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EV_HAUL:
        {
            if (e->getHaulSource()->isExData("view") &&
                !hasChildren())
            {
                is_haul_in_ = true;
                requestDraw();
                consumed = true;
            }
            break;
        }

        case InputEvent::EV_HAUL_END:
        {
            if (e->getHaulSource()->isExData("view")) {
                is_haul_in_ = false;
                requestDraw();
                consumed = true;

                if (!hasChildren()) {
                    auto view = LayoutParser::createView(
                        e->getHaulSource()->getRaw(), getContext(), {});

                    if (isLayouter(e->getHaulSource()->getRaw())) {
                        view->setLayoutSize(LS_FILL, LS_FILL);
                        vd_.reset(obtainViewDelegate(e->getHaulSource()->getRaw()));
                        view->setDelegate(vd_.get());
                        addView(view);
                        mode_ = Mode::Layout;
                    } else {
                        view->setLayoutSize(LS_AUTO, LS_AUTO);
                        addView(view);
                        mode_ = Mode::View;
                    }
                }
            }
            break;
        }

        case InputEvent::EV_HAUL_LEAVE:
            is_haul_in_ = false;
            requestDraw();
            consumed = true;
            break;

        case InputEvent::EVM_DOWN:
            if (mode_ == Mode::View && hasChildren()) {
                if (getChildAt(0)->getBounds().hit(e->getPos())) {
                    is_view_selected_ = !is_view_selected_;
                    requestDraw();
                }
            }
            break;

        default:
            break;
        }

        return true;
    }

}
