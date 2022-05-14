// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/event/keyboard.h"

#include <Windows.h>


namespace ukive {

    // static
    bool Keyboard::mapKey(int raw_key, int* key) {
        if (raw_key >= 48 && raw_key <= 57) {
            *key = KEY_0 + raw_key - 48;
            return true;
        }
        if (raw_key >= 65 && raw_key <= 90) {
            *key = KEY_A + raw_key - 65;
            return true;
        }

        switch (raw_key) {
        case VK_LEFT:    *key = KEY_LEFT;      return true;
        case VK_UP:      *key = KEY_UP;        return true;
        case VK_RIGHT:   *key = KEY_RIGHT;     return true;
        case VK_DOWN:    *key = KEY_DOWN;      return true;
        case VK_SHIFT:   *key = KEY_SHIFT;     return true;
        case VK_CONTROL: *key = KEY_CONTROL;   return true;
        case VK_TAB:     *key = KEY_TAB;       return true;
        case VK_RETURN:  *key = KEY_ENTER;     return true;
        case VK_MENU:    *key = KEY_ALT;       return true;
        case VK_ESCAPE:  *key = KEY_ESC;       return true;
        case VK_CAPITAL: *key = KEY_CAPS;      return true;
        case VK_BACK:    *key = KEY_BACKSPACE; return true;
        case VK_SPACE:   *key = KEY_SPACE;     return true;
        case VK_HOME:    *key = KEY_HOME;      return true;
        case VK_END:     *key = KEY_END;       return true;
        case VK_INSERT:  *key = KEY_INSERT;    return true;
        case VK_DELETE:  *key = KEY_DELETE;    return true;
        case VK_NAVIGATION_DOWN:  *key = KEY_PAGE_DOWN; return true;
        case VK_NAVIGATION_UP:    *key = KEY_PAGE_UP;   return true;
        default: break;
        }
        return false;
    }

    // static
    bool Keyboard::unmapKey(int key, int* raw_key) {
        if (key >= KEY_0 && key <= KEY_9) {
            *raw_key = 48 + key - KEY_0;
            return true;
        }
        if (key >= KEY_A && key <= KEY_Z) {
            *raw_key = 65 + key - KEY_A;
            return true;
        }

        switch (key) {
        case KEY_LEFT:      *raw_key = VK_LEFT;    return true;
        case KEY_UP:        *raw_key = VK_UP;      return true;
        case KEY_RIGHT:     *raw_key = VK_RIGHT;   return true;
        case KEY_DOWN:      *raw_key = VK_DOWN;    return true;
        case KEY_SHIFT:     *raw_key = VK_SHIFT;   return true;
        case KEY_CONTROL:   *raw_key = VK_CONTROL; return true;
        case KEY_TAB:       *raw_key = VK_TAB;     return true;
        case KEY_ENTER:     *raw_key = VK_RETURN;  return true;
        case KEY_ALT:       *raw_key = VK_MENU;    return true;
        case KEY_ESC:       *raw_key = VK_ESCAPE;  return true;
        case KEY_CAPS:      *raw_key = VK_CAPITAL; return true;
        case KEY_BACKSPACE: *raw_key = VK_BACK;    return true;
        case KEY_SPACE:     *raw_key = VK_SPACE;   return true;
        case KEY_HOME:      *raw_key = VK_HOME;    return true;
        case KEY_END:       *raw_key = VK_END;     return true;
        case KEY_INSERT:    *raw_key = VK_INSERT;  return true;
        case KEY_DELETE:    *raw_key = VK_DELETE;  return true;
        case KEY_PAGE_UP:   *raw_key = VK_NAVIGATION_DOWN; return true;
        case KEY_PAGE_DOWN: *raw_key = VK_NAVIGATION_UP;   return true;
        default: break;
        }
        return false;
    }

    // static
    bool Keyboard::filterChars(const std::u16string_view& chars) {
        if (chars.empty()) {
            return true;
        }
        if (chars.size() != 1) {
            return false;
        }

        // https://docs.microsoft.com/en-us/windows/win32/inputdev/using-keyboard-input
        auto ch = chars[0];
        if (ch == 0x08 || ch == 0x0A || ch == 0x1B || ch == 0x09 || ch == 0x0D) {
            return true;
        }
        return false;
    }

    // static
    bool Keyboard::isKeyPressed(int key) {
        int virtual_key;
        if (!unmapKey(key, &virtual_key)) {
            return false;
        }
        return ::GetKeyState(virtual_key) < 0;
    }

    // static
    bool Keyboard::isKeyToggled(int key) {
        int virtual_key;
        if (!unmapKey(key, &virtual_key)) {
            return false;
        }
        return (::GetKeyState(virtual_key) & 1) == 1;
    }

}
