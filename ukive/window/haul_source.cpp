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
          delegate_(d)
    {
        data_.type = DT_NONE;
    }

    void HaulSource::ignite(View* v, InputEvent* e) {
        if (!v || !e || !v->getWindow()) {
            return;
        }
        if (is_hauling_) {
            return;
        }
        if (!delegate_->canHaul(this)) {
            return;
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
        case InputEvent::EVT_DOWN:
            if (v->isPressed()) {
                start_pos_ = e->getPos();
            }
            break;

        case InputEvent::EVM_MOVE:
        case InputEvent::EVT_MOVE:
            if (v->isPressed()) {
                auto d = e->getPos() - start_pos_;
                int r = v->getContext().dp2pxi(kMouseDraggingThreshold);
                if (d.lengsq() > r * r) {
                    is_hauling_ = true;
                    window_ = v->getWindow();
                    window_->startHaul(this);
                    delegate_->onHaulStarted(this, v, e);
                }
            }
            break;

        default:
            break;
        }
    }

    bool HaulSource::brake(InputEvent* e) {
        if (!is_hauling_) {
            return true;
        }

        if (!delegate_->onHauling(this, e)) {
            stop();
            return true;
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_UP:
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                stop();
                return true;
            }
            break;

        default:
            break;
        }

        return false;
    }

    void HaulSource::stop() {
        if (is_hauling_) {
            is_hauling_ = false;
            window_->stopHaul(this);
            delegate_->onHaulStopped(this);
        }
    }

    void HaulSource::cancel() {
        if (is_hauling_) {
            is_hauling_ = false;
            window_->stopHaul(this);
            delegate_->onHaulCancelled(this);
        }
    }


    void HaulSource::setURL(const std::string_view& url) {
        data_.type = DT_URL;
        data_.raw = url;
    }

    void HaulSource::setText(const std::string_view& text) {
        data_.type = DT_TEXT;
        data_.raw = text;
    }

    void HaulSource::setBytes(const std::string_view& bytes) {
        data_.type = DT_BYTES;
        data_.raw = bytes;
    }

    void HaulSource::setExData(
        const std::string_view& exid,
        const std::string_view& data)
    {
        data_.type = DT_EX_DATA;
        data_.exd = exid;
        data_.raw = data;
    }

    const std::string& HaulSource::getRaw() const {
        return data_.raw;
    }

    bool HaulSource::isURL() const {
        return data_.type == DT_URL;
    }

    bool HaulSource::isText() const {
        return data_.type == DT_TEXT;
    }

    bool HaulSource::isBytes() const {
        return data_.type == DT_BYTES;
    }

    bool HaulSource::isExData(const std::string_view& exd) const {
        return data_.type == DT_EX_DATA && data_.exd == exd;
    }

    int HaulSource::getSourceId() const {
        return id_;
    }

}
