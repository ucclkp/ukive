// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_EVENT_KEYBOARD_H_
#define UKIVE_EVENT_KEYBOARD_H_

#include <string>


namespace ukive {

    class Keyboard {
    public:
        enum Key {
            KEY_0 = 48,
            KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

            KEY_A = 65,
            KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
            KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
            KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

            KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,
            KEY_F9, KEY_F10, KEY_F11, KEY_F12,

            KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN,

            KEY_SHIFT,
            KEY_CONTROL,
            KEY_TAB,
            KEY_ENTER,
            KEY_ALT,
            KEY_ESC,
            KEY_CAPS,
            KEY_BACKSPACE,
            KEY_SPACE,
            KEY_HOME,
            KEY_END,
            KEY_INSERT,
            KEY_DELETE,
            KEY_PAGE_UP,
            KEY_PAGE_DOWN,
        };

        static bool mapKey(int raw_key, int* key);
        static bool unmapKey(int key, int* raw_key);

        /**
         * 过滤字符。
         * 如果当前字符为空或不可打印，返回 true；
         * 否则返回 false。
         */
        static bool filterChars(const std::u16string& chars);

        static bool isKeyPressed(int key);
        static bool isKeyToggled(int key);
    };

}

#endif  // UKIVE_EVENT_KEYBOARD_H_
