// Copyright (c) 2022 ucclkp <ucclkp@gmail.com>.
// This file is part of vevah project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "sequence_layout_delegate.h"

#include "utils/log.h"

#include "ukive/graphics/canvas.h"
#include "ukive/resources/layout_parser.h"
#include "ukive/views/layout/layout_view.h"
#include "ukive/views/layout/sequence_layout.h"
#include "ukive/window/context.h"
#include "ukive/window/haul_source.h"

#include "vevah/view_selected_listener.h"


namespace vevah {

    SequenceLayoutDelegate::SequenceLayoutDelegate(OnViewSelectedListener* l)
        : listener_(l) {}

    bool SequenceLayoutDelegate::onHookInputReceived(
        ukive::LayoutView* v, ukive::InputEvent* e, bool* ret)
    {
        *ret = true;
        return true;
    }

    bool SequenceLayoutDelegate::onInputReceived(
        ukive::View* v, ukive::InputEvent* e, bool* ret)
    {
        ubassert(typeid(*v) == typeid(ukive::SequenceLayout));
        auto sl = static_cast<ukive::SequenceLayout*>(v);

        bool consumed = false;

        switch (e->getEvent()) {
        case ukive::InputEvent::EV_HAUL:
        {
            if (e->getHaulSource()->isExData("view")) {
                determinePlacement(sl, e->getPos());
                v->requestDraw();
                consumed = true;
            }
            break;
        }

        case ukive::InputEvent::EV_HAUL_END:
        {
            if (e->getHaulSource()->isExData("view")) {
                if (placement_.type != PlaceType::None) {
                    placement_.type = PlaceType::None;
                    v->requestDraw();
                    consumed = true;

                    auto view = ukive::LayoutParser::createView(
                        e->getHaulSource()->getRaw(), v->getContext(), {});
                    view->setLayoutSize(ukive::View::LS_AUTO, ukive::View::LS_AUTO);
                    sl->addView(placement_.insert_pos, view);
                }
            }
            break;
        }

        case ukive::InputEvent::EV_HAUL_LEAVE:
            placement_.type = PlaceType::None;
            v->requestDraw();
            consumed = true;
            break;

        case ukive::InputEvent::EVM_DOWN:
        {
            ubassert(typeid(*v) == typeid(ukive::SequenceLayout));
            auto sl = static_cast<ukive::SequenceLayout*>(v);

            for (size_t i = 0; i < sl->getChildCount(); ++i) {
                if (sl->getChildAt(i)->getBounds().hit(e->getPos())) {
                    if (sel_.valid && sel_.pos == i) {
                        sel_.valid = false;
                        if (listener_) listener_->onViewSelected(nullptr);
                    } else {
                        sel_.pos = i;
                        sel_.valid = true;
                        if (listener_) listener_->onViewSelected(sl->getChildAt(i));
                    }
                    sl->requestDraw();
                }
            }
            break;
        }

        default:
            break;
        }

        *ret = true;
        return true;
    }

    void SequenceLayoutDelegate::onDrawOverChildrenReceived(
        ukive::View* v, ukive::Canvas* c)
    {
        ubassert(typeid(*v) == typeid(ukive::SequenceLayout));
        auto sl = static_cast<ukive::SequenceLayout*>(v);
        auto ctx = v->getContext();

        if (placement_.type == PlaceType::Next) {
            ukive::RectF rect;
            rect.pos(ukive::PointF(placement_.place.pos()));
            rect.size(ctx.dp2px(100), ctx.dp2px(100));
            c->fillRect(rect, ukive::Color::Pink400);
        } else if (placement_.type == PlaceType::Insert) {
            ukive::PointF ps = ukive::PointF(placement_.place.pos());
            ukive::PointF pe = ukive::PointF(placement_.place.pos_rt());
            c->drawLine(ps, pe, ctx.dp2px(2), ukive::Color::Pink400);
        }

        if (sl->hasChildren() &&
            sel_.valid &&
            sel_.pos < sl->getChildCount())
        {
            auto bounds = sl->getChildAt(sel_.pos)->getBounds();
            c->drawRect(ukive::RectF(bounds), ctx.dp2px(3), ukive::Color::Blue500);
        }
    }

    void SequenceLayoutDelegate::determinePlacement(
        ukive::SequenceLayout* v, const ukive::Point& p)
    {
        bool hit = false;
        bool is_vert = v->getOrientation() == ukive::SequenceLayout::VERTICAL;
        for (size_t i = 0; i < v->getChildCount(); ++i) {
            auto child = v->getChildAt(i);
            bool cond;
            if (is_vert) {
                cond = p.y() >= child->getY() && p.y() < child->getBottom();
            } else {
                cond = p.x() >= child->getX() && p.x() < child->getRight();
            }
            if (cond) {
                placement_.type = PlaceType::Insert;
                placement_.insert_pos = i;
                placement_.place = child->getBounds();
                hit = true;
                break;
            }
        }

        if (!hit) {
            placement_.type = PlaceType::Next;
            auto count = v->getChildCount();
            if (count > 0) {
                placement_.insert_pos = count;
                if (is_vert) {
                    auto y = v->getChildAt(count - 1)->getBottom();
                    placement_.place.pos(0, y);
                } else {
                    auto x = v->getChildAt(count - 1)->getRight();
                    placement_.place.pos(x, 0);
                }
            } else {
                placement_.insert_pos = 0;
                placement_.place.pos(0, 0);
            }
            placement_.place.size(0, 0);
        }
    }

}
