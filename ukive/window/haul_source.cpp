// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "haul_source.h"

#include "window.h"
#include "ukive/event/input_consts.h"
#include "ukive/event/input_event.h"
#include "ukive/views/view.h"
#include "ukive/window/haul_delegate.h"


namespace ukive {

    HaulSource::HaulSource(int id, HaulDelegate* d)
        : id_(id),
          delegate_(d) {}

    void HaulSource::ignite(View* v, InputEvent* e) {
        if (is_dragging_) {
            return;
        }
        if (!delegate_->canHaul(this)) {
            return;
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (v->isPressed()) {
                start_pos_ = e->getPos();
            }
            break;

        case InputEvent::EVM_MOVE:
            if (v->isPressed()) {
                auto d = e->getPos() - start_pos_;
                int r = v->getContext().dp2pxi(kMouseDraggingThreshold);
                if (d.lengsq() > r * r) {
                    v->getWindow()->startHaul(this);
                    is_dragging_ = true;
                }
            }
            break;

        default:
            break;
        }
    }

    bool HaulSource::brake(InputEvent* e) {
        delegate_->onHauling(this, e);

        switch (e->getEvent()) {
        case InputEvent::EVM_UP:
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                return true;
            }
            break;

        default:
            break;
        }
        return false;
    }

    void HaulSource::cancel() {
        delegate_->onHaulStopped(this);
    }

}
