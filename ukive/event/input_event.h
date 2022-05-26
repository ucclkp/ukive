// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_EVENT_INPUT_EVENT_H_
#define UKIVE_EVENT_INPUT_EVENT_H_

#include <functional>
#include <map>
#include <string>

#include "ukive/graphics/point.hpp"


namespace ukive {

    class HaulSource;

    class InputEvent {
    public:
        enum {
            EV_NONE = 0,
            EV_HAUL,
            EV_HAUL_END,
            EV_HAUL_LEAVE,
            EV_LEAVE,

            // 鼠标事件
            EVM_DOWN,
            EVM_UP,
            EVM_MOVE,
            EVM_WHEEL,
            EVM_HOVER,

            // 该事件不会分发给 View
            EVM_LEAVE_WIN,

            // 触摸事件
            EVT_DOWN,
            EVT_MULTI_DOWN,
            EVT_MULTI_UP,
            EVT_UP,
            EVT_MOVE,

            // 键盘事件
            // 键盘按键使用 keyboard.h 中的定义。
            EVK_DOWN,
            EVK_UP,
            EVK_CHARS,
        };

        // 鼠标按键定义
        enum {
            MK_LEFT = 0,
            MK_MIDDLE,
            MK_RIGHT,
            MK_XBUTTON_1,
            MK_XBUTTON_2,
        };


        enum PointerType {
            PT_NONE,
            PT_MOUSE,
            PT_TOUCH,
            PT_PEN,
            PT_KEYBOARD,
        };

        enum WheelGranularity {
            WG_DELTA,
            WG_HIGH_PRECISE,
        };

        struct InputPos {
            Point pos{};
            Point raw{};
        };

        InputEvent();
        InputEvent(const InputEvent& source) = default;
        ~InputEvent();

        void setEvent(int ev);
        void setPointerType(int type);
        void setX(int x);
        void setY(int y);
        void setPos(const Point& pos);
        void setX(int x, int id);
        void setY(int y, int id);
        void setPos(const Point& pos, int id);
        void setRawX(int raw_x);
        void setRawY(int raw_y);
        void setRawPos(const Point& pos);
        void setRawX(int raw_x, int id);
        void setRawY(int raw_y, int id);
        void setRawPos(const Point& pos, int id);
        void setWheelValue(int wheel, WheelGranularity granularity);
        void setMouseKey(int key);
        void setKeyboardChars(std::u16string chars, bool repeat);
        void setKeyboardKey(int key, bool repeat);
        void setCurTouchId(int id);
        void setOutside(bool outside);
        void setIsNoDispatch(bool captured);
        void setCancelled(bool cancelled);
        void setPrimaryTouchDown(bool primary);
        void setHaulSource(HaulSource* src);

        void offsetInputPos(int dx, int dy);
        void transformInputPos(std::function<void(InputPos* pos)>&& func);

        int getEvent() const;
        int getPointerType() const;
        int getX() const;
        int getY() const;
        Point getPos() const;
        int getX(int id) const;
        int getY(int id) const;
        Point getPos(int id) const;
        int getRawX() const;
        int getRawY() const;
        Point getRawPos() const;
        int getRawX(int id) const;
        int getRawY(int id) const;
        Point getRawPos(int id) const;
        int getWheelValue() const;
        WheelGranularity getWheelGranularity() const;
        int getMouseKey() const;
        const std::u16string& getKeyboardChars() const;
        int getKeyboardKey() const;
        int getCurTouchId() const;
        HaulSource* getHaulSource() const;

        bool isMouseEvent() const;
        bool isTouchEvent() const;
        bool isKeyboardEvent() const;
        bool isKeyRepeat() const;
        bool isNoDispatch() const;
        bool isCancelled() const;
        bool isPrimaryTouchDown() const;

        /**
         * 当鼠标事件发生于 View 外部时，该方法返回 true。
         * 只有当 View 的 setReceiveOutsideInputEvent() 方法以 true 为参数
         * 调用之后，此方法才有效。
         */
        bool isOutside() const;

        void combineTouchEvent(InputEvent* e);
        bool hasTouchEvent(InputEvent* e) const;

        void clearTouchUp();
        void clearTouch();

        std::string toString() const;
        std::string evtos() const;
        std::string mktos() const;
        std::string pttos() const;

    private:
        int wheel_;
        WheelGranularity granularity_;

        int mouse_key_;
        InputPos mouse_pos_;
        std::map<int, InputPos> touch_pos_;
        int cur_touch_id_;

        std::u16string chars_;
        int key_;
        bool is_repeat_;

        int event_type_;
        int pointer_type_;

        bool is_outside_;
        bool is_no_dispatch_;
        bool is_cancelled_ = false;
        bool is_primary_touch_down_ = false;

        HaulSource* haul_ = nullptr;
    };

}

#endif  // UKIVE_EVENT_INPUT_EVENT_H_
