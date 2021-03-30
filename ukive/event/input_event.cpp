// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/event/input_event.h"

#include "utils/log.h"


namespace ukive {

    InputEvent::InputEvent()
        : wheel_(0),
          granularity_(WG_DELTA),
          mouse_key_(0),
          cur_touch_id_(-1),
          key_(0),
          is_repeat_(false),
          event_type_(EV_NONE),
          pointer_type_(PT_NONE),
          is_outside_(false),
          is_no_dispatch_(false) {
    }

    InputEvent::~InputEvent() {
    }

    void InputEvent::setEvent(int ev) {
        event_type_ = ev;
    }

    void InputEvent::setPointerType(int type) {
        pointer_type_ = type;
    }

    void InputEvent::setX(int x) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.x = x;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].x = x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setY(int y) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.y = y;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].y = y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setX(int x, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].x = x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setY(int y, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].y = y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawX(int raw_x) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw_x = raw_x;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw_x = raw_x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawY(int raw_y) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw_y = raw_y;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw_y = raw_y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawX(int raw_x, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw_x = raw_x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawY(int raw_y, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw_y = raw_y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setWheelValue(int wheel, WheelGranularity granularity) {
        wheel_ = wheel;
        granularity_ = granularity;
    }

    void InputEvent::setMouseKey(int key) {
        mouse_key_ = key;
    }

    void InputEvent::setKeyboardChars(std::u16string chars, bool repeat) {
        chars_ = std::move(chars);
        is_repeat_ = repeat;
    }

    void InputEvent::setKeyboardKey(int key, bool repeat) {
        key_ = key;
        is_repeat_ = repeat;
    }

    void InputEvent::setCurTouchId(int id) {
        cur_touch_id_ = id;
    }

    void InputEvent::setOutside(bool outside) {
        is_outside_ = outside;
    }

    void InputEvent::setIsNoDispatch(bool captured) {
        is_no_dispatch_ = captured;
    }

    void InputEvent::setCancelled(bool cancelled) {
        is_cancelled_ = cancelled;
    }

    void InputEvent::setPrimaryTouchDown(bool primary) {
        is_primary_touch_down_ = primary;
    }

    void InputEvent::offsetInputPos(int dx, int dy) {
        if (event_type_ == EV_LEAVE_VIEW) {
            return;
        }

        if (isMouseEvent()) {
            mouse_pos_.x += dx;
            mouse_pos_.y += dy;
        } else if (isTouchEvent()) {
            for (auto& pos : touch_pos_) {
                pos.second.x += dx;
                pos.second.y += dy;
            }
        }
    }

    void InputEvent::transformInputPos(std::function<void(InputPos* pos)>&& func) {
        if (event_type_ == EV_LEAVE_VIEW) {
            return;
        }

        if (isMouseEvent()) {
            func(&mouse_pos_);
        } else if (isTouchEvent()) {
            for (auto& pos : touch_pos_) {
                func(&pos.second);
            }
        }
    }

    int InputEvent::getEvent() const {
        return event_type_;
    }

    int InputEvent::getPointerType() const {
        return pointer_type_;
    }

    int InputEvent::getX() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.x;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getY() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.y;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getX(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getY(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawX() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw_x;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw_x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawY() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw_y;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw_y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawX(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw_x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawY(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw_y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getWheelValue() const {
        return wheel_;
    }

    InputEvent::WheelGranularity InputEvent::getWheelGranularity() const {
        return granularity_;
    }

    int InputEvent::getMouseKey() const {
        return mouse_key_;
    }

    const std::u16string& InputEvent::getKeyboardChars() const {
        return chars_;
    }

    int InputEvent::getKeyboardKey() const {
        return key_;
    }

    int InputEvent::getCurTouchId() const {
        return cur_touch_id_;
    }

    bool InputEvent::isMouseEvent() const {
        DCHECK(event_type_ == EV_LEAVE_VIEW || pointer_type_ != PT_NONE);
        return pointer_type_ == PT_MOUSE;
    }

    bool InputEvent::isTouchEvent() const {
        DCHECK(event_type_ == EV_LEAVE_VIEW || pointer_type_ != PT_NONE);
        return pointer_type_ == PT_TOUCH;
    }

    bool InputEvent::isKeyboardEvent() const {
        DCHECK(event_type_ == EV_LEAVE_VIEW || pointer_type_ != PT_NONE);
        return pointer_type_ == PT_KEYBOARD;
    }

    bool InputEvent::isKeyRepeat() const {
        return is_repeat_;
    }

    bool InputEvent::isNoDispatch() const {
        return is_no_dispatch_;
    }

    bool InputEvent::isCancelled() const {
        return is_cancelled_;
    }

    bool InputEvent::isPrimaryTouchDown() const {
        return is_primary_touch_down_;
    }

    bool InputEvent::isOutside() const {
        return is_outside_;
    }

    void InputEvent::combineTouchEvent(InputEvent* e) {
        if (!e->isTouchEvent()) {
            DCHECK(false);
            return;
        }

        if (event_type_ == EV_NONE) {
            *this = *e;
        } else if (isTouchEvent()) {
            switch (e->getEvent()) {
            case EVT_DOWN:
                DCHECK(touch_pos_.find(e->getCurTouchId()) == touch_pos_.end());
                if (e->isPrimaryTouchDown()) {
                    DCHECK(touch_pos_.empty());
                    touch_pos_.clear();
                }
                touch_pos_[e->getCurTouchId()] = { e->getX(), e->getY(), e->getRawX(), e->getRawY() };
                event_type_ = touch_pos_.size() > 1 ? EVT_MULTI_DOWN : EVT_DOWN;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EVT_UP:
                DCHECK(touch_pos_.find(e->getCurTouchId()) != touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getX(), e->getY(), e->getRawX(), e->getRawY() };
                event_type_ = touch_pos_.size() > 1 ? EVT_MULTI_UP : EVT_UP;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EVT_MOVE:
                DCHECK(touch_pos_.find(e->getCurTouchId()) != touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getX(), e->getY(), e->getRawX(), e->getRawY() };
                event_type_ = EVT_MOVE;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EV_LEAVE_VIEW:
                event_type_ = e->getEvent();
                break;

            default:
                DCHECK(false);
                break;
            }
        } else {
            DCHECK(false);
        }
    }

    bool InputEvent::hasTouchEvent(InputEvent* e) const {
        if (!e->isTouchEvent()) {
            DCHECK(false);
            return false;
        }

        if (e->getEvent() == EVT_MOVE ||
            e->getEvent() == EVT_UP)
        {
            return touch_pos_.find(e->getCurTouchId()) != touch_pos_.end();
        }

        DCHECK(false);
        return false;
    }

    void InputEvent::clearTouchUp() {
        if (!isTouchEvent()) {
            DCHECK(false);
            return;
        }

        if (event_type_ == EVT_UP ||
            event_type_ == EVT_MULTI_UP)
        {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                touch_pos_.erase(it);
            } else {
                DCHECK(false);
            }
        } else if (event_type_ == EV_LEAVE_VIEW) {
            touch_pos_.clear();
            cur_touch_id_ = -1;
        }
    }

    void InputEvent::clearTouch() {
        touch_pos_.clear();
        cur_touch_id_ = -1;
    }

}
