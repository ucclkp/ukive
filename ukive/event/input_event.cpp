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
            mouse_pos_.pos.x(x);
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].pos.x(x);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setY(int y) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.pos.y(y);
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].pos.y(y);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setPos(const Point& pos) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.pos = pos;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].pos = pos;
            return;
        }
        ubassert(false);
    }

    void InputEvent::setX(int x, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].pos.x(x);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setY(int y, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].pos.y(y);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setPos(const Point& pos, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].pos = pos;
            return;
        }
        ubassert(false);
    }

    void InputEvent::setRawX(int raw_x) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw.x(raw_x);
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw.x(raw_x);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setRawY(int raw_y) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw.y(raw_y);
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw.y(raw_y);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setRawPos(const Point& pos) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw = pos;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw = pos;
            return;
        }
        ubassert(false);
    }

    void InputEvent::setRawX(int raw_x, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw.x(raw_x);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setRawY(int raw_y, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw.y(raw_y);
            return;
        }
        ubassert(false);
    }

    void InputEvent::setRawPos(const Point& pos, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw = pos;
            return;
        }
        ubassert(false);
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
        if (event_type_ == EV_LEAVE) {
            return;
        }

        if (isMouseEvent()) {
            mouse_pos_.pos.x() += dx;
            mouse_pos_.pos.y() += dy;
        } else if (isTouchEvent()) {
            for (auto& pos : touch_pos_) {
                pos.second.pos.x() += dx;
                pos.second.pos.y() += dy;
            }
        }
    }

    void InputEvent::transformInputPos(std::function<void(InputPos* pos)>&& func) {
        if (event_type_ == EV_LEAVE) {
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
            return mouse_pos_.pos.x();
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.pos.x();
            }
        }
        ubassert(false);
        return 0;
    }

    int InputEvent::getY() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.pos.y();
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.pos.y();
            }
        }
        ubassert(false);
        return 0;
    }

    Point InputEvent::getPos() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.pos;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.pos;
            }
        }
        ubassert(false);
        return {};
    }

    int InputEvent::getX(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.pos.x();
            }
        }
        ubassert(false);
        return 0;
    }

    int InputEvent::getY(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.pos.y();
            }
        }
        ubassert(false);
        return 0;
    }

    Point InputEvent::getPos(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.pos;
            }
        }
        ubassert(false);
        return {};
    }

    int InputEvent::getRawX() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw.x();
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw.x();
            }
        }
        ubassert(false);
        return 0;
    }

    int InputEvent::getRawY() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw.y();
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw.y();
            }
        }
        ubassert(false);
        return 0;
    }

    Point InputEvent::getRawPos() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw;
            }
        }
        ubassert(false);
        return {};
    }

    int InputEvent::getRawX(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw.x();
            }
        }
        ubassert(false);
        return 0;
    }

    int InputEvent::getRawY(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw.y();
            }
        }
        ubassert(false);
        return 0;
    }

    Point InputEvent::getRawPos(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw;
            }
        }
        ubassert(false);
        return {};
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
        ubassert(
            event_type_ == EV_LEAVE ||
            pointer_type_ != PT_NONE);
        return pointer_type_ == PT_MOUSE;
    }

    bool InputEvent::isTouchEvent() const {
        ubassert(
            event_type_ == EV_LEAVE ||
            pointer_type_ != PT_NONE);
        return pointer_type_ == PT_TOUCH;
    }

    bool InputEvent::isKeyboardEvent() const {
        ubassert(
            event_type_ == EV_LEAVE ||
            pointer_type_ != PT_NONE);
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
            ubassert(false);
            return;
        }

        if (event_type_ == EV_NONE) {
            *this = *e;
        } else if (isTouchEvent()) {
            switch (e->getEvent()) {
            case EVT_DOWN:
                ubassert(touch_pos_.find(e->getCurTouchId()) == touch_pos_.end());
                if (e->isPrimaryTouchDown()) {
                    ubassert(touch_pos_.empty());
                    touch_pos_.clear();
                }
                touch_pos_[e->getCurTouchId()] = { e->getPos(), e->getRawPos() };
                event_type_ = touch_pos_.size() > 1 ? EVT_MULTI_DOWN : EVT_DOWN;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EVT_UP:
                ubassert(touch_pos_.find(e->getCurTouchId()) != touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getPos(), e->getRawPos() };
                event_type_ = touch_pos_.size() > 1 ? EVT_MULTI_UP : EVT_UP;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EVT_MOVE:
                ubassert(touch_pos_.find(e->getCurTouchId()) != touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getPos(), e->getRawPos() };
                event_type_ = EVT_MOVE;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EV_DRAG:
            case EV_DRAG_END:
            case EV_LEAVE:
                event_type_ = e->getEvent();
                break;

            default:
                ubassert(false);
                break;
            }
        } else {
            ubassert(false);
        }
    }

    bool InputEvent::hasTouchEvent(InputEvent* e) const {
        if (!e->isTouchEvent()) {
            ubassert(false);
            return false;
        }

        if (e->getEvent() == EVT_MOVE ||
            e->getEvent() == EVT_UP)
        {
            return touch_pos_.find(e->getCurTouchId()) != touch_pos_.end();
        }

        ubassert(false);
        return false;
    }

    void InputEvent::clearTouchUp() {
        if (!isTouchEvent()) {
            ubassert(false);
            return;
        }

        if (event_type_ == EVT_UP ||
            event_type_ == EVT_MULTI_UP)
        {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                touch_pos_.erase(it);
            } else {
                ubassert(false);
            }
        } else if (event_type_ == EV_LEAVE) {
            touch_pos_.clear();
            cur_touch_id_ = -1;
        }
    }

    void InputEvent::clearTouch() {
        touch_pos_.clear();
        cur_touch_id_ = -1;
    }

    std::string InputEvent::toString() const {
        std::string r;
        r.append(evtos());

        switch (event_type_) {
        case EV_NONE:
        case EV_LEAVE:
        case EVM_LEAVE_WIN:
            break;

        case EV_DRAG:
        case EV_DRAG_END:
        case EVM_DOWN:
        case EVM_UP:
        case EVM_MOVE:
        case EVM_WHEEL:
        case EVM_HOVER:
        case EVT_DOWN:
        case EVT_MULTI_DOWN:
        case EVT_MULTI_UP:
        case EVT_UP:
        case EVT_MOVE:
            r += utl::usformat(" (%d, %d)", getX(), getY());
            break;

        case EVK_DOWN:
        case EVK_UP:
            r += utl::usformat(" %d", key_);
            break;

        case EVK_CHARS:
            r += utl::usformat(" %s", chars_);
            break;

        default:
            break;
        }

        return r;
    }

    std::string InputEvent::evtos() const {
        switch (event_type_) {
            // Misc
        case EV_NONE:   return "EV_NONE";
        case EV_DRAG:   return "EV_DRAG";
        case EV_DRAG_END: return "EV_DRAG_END";
        case EV_LEAVE:  return "EV_LEAVE";

            // Mouse
        case EVM_DOWN:  return "EVM_DOWN";
        case EVM_UP:    return "EVM_UP";
        case EVM_MOVE:  return "EVM_MOVE";
        case EVM_WHEEL: return "EVM_WHEEL";
        case EVM_LEAVE_WIN:  return "EVM_LEAVE_WIN";
        case EVM_HOVER:      return "EVM_HOVER";

            // Touch
        case EVT_DOWN:  return "EVT_DOWN";
        case EVT_MULTI_DOWN: return "EVT_MULTI_DOWN";
        case EVT_MULTI_UP:   return "EVT_MULTI_UP";
        case EVT_UP:    return "EVT_UP";
        case EVT_MOVE:  return "EVT_MOVE";

            // Keyboard
        case EVK_DOWN:  return "EVK_DOWN";
        case EVK_UP:    return "EVK_UP";
        case EVK_CHARS: return "EVK_CHARS";

        default:        return "EV_UNKNOWN";
        }
    }

    std::string InputEvent::mktos() const {
        switch (mouse_key_) {
        case MK_LEFT:      return "MK_LEFT";
        case MK_MIDDLE:    return "MK_MIDDLE";
        case MK_RIGHT:     return "MK_RIGHT";
        case MK_XBUTTON_1: return "MK_XB1";
        case MK_XBUTTON_2: return "MK_XB2";

        default: return "MK_UNKNOWN";
        }
    }

    std::string InputEvent::pttos() const {
        switch (pointer_type_) {
        case PT_NONE:     return "PT_NONE";
        case PT_MOUSE:    return "PT_MOUSE";
        case PT_TOUCH:    return "PT_TOUCH";
        case PT_PEN:      return "PT_PEN";
        case PT_KEYBOARD: return "PT_KEYBOARD";

        default: return "PT_UNKNOWN";
        }
    }

}
