// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/event/keyboard.h"

#import <Cocoa/Cocoa.h>

#define MAP_MAC_KEY(raw, k)  \
    case raw: *key = k; return true;

#define UNMAP_MAC_KEY(raw, k)  \
    case k: *raw_key = raw; return true;


namespace ukive {

    // static
    bool Keyboard::mapKey(int raw_key, int* key) {
        // https://stackoverflow.com/questions/2080312/where-can-i-find-a-list-of-key-codes-for-use-with-cocoas-nsevent-class
        switch (raw_key) {
            MAP_MAC_KEY(0, KEY_A)          MAP_MAC_KEY(1, KEY_S)       MAP_MAC_KEY(2, KEY_D)
            MAP_MAC_KEY(3, KEY_F)          MAP_MAC_KEY(4, KEY_H)       MAP_MAC_KEY(5, KEY_G)
            MAP_MAC_KEY(6, KEY_Z)          MAP_MAC_KEY(7, KEY_X)       MAP_MAC_KEY(8, KEY_C)
            MAP_MAC_KEY(9, KEY_V)          /*MAP_MAC_KEY(10, )*/       MAP_MAC_KEY(11, KEY_B)
            MAP_MAC_KEY(12, KEY_Q)         MAP_MAC_KEY(13, KEY_W)      MAP_MAC_KEY(14, KEY_E)
            MAP_MAC_KEY(15, KEY_R)         MAP_MAC_KEY(16, KEY_Y)      MAP_MAC_KEY(17, KEY_T)
            MAP_MAC_KEY(18, KEY_1)         MAP_MAC_KEY(19, KEY_2)      MAP_MAC_KEY(20, KEY_3)
            MAP_MAC_KEY(21, KEY_4)         MAP_MAC_KEY(22, KEY_6)      MAP_MAC_KEY(23, KEY_5)
            /*MAP_MAC_KEY(24, )*/          MAP_MAC_KEY(25, KEY_9)      MAP_MAC_KEY(26, KEY_7)
            /*MAP_MAC_KEY(27, )*/          MAP_MAC_KEY(28, KEY_8)      MAP_MAC_KEY(29, KEY_0)
            /*MAP_MAC_KEY(30, )*/          MAP_MAC_KEY(31, KEY_O)      MAP_MAC_KEY(32, KEY_U)
            /*MAP_MAC_KEY(33, )*/          MAP_MAC_KEY(34, KEY_I)      MAP_MAC_KEY(35, KEY_P)
            MAP_MAC_KEY(36, KEY_ENTER)     MAP_MAC_KEY(37, KEY_L)      MAP_MAC_KEY(38, KEY_J)
            /*MAP_MAC_KEY(39, )*/          MAP_MAC_KEY(40, KEY_K)      /*MAP_MAC_KEY(41, )*/
            /*MAP_MAC_KEY(42, )*/          /*MAP_MAC_KEY(43, )*/       /*MAP_MAC_KEY(44, )*/
            MAP_MAC_KEY(45, KEY_N)         MAP_MAC_KEY(46, KEY_M)      /*MAP_MAC_KEY(47, )*/
            MAP_MAC_KEY(48, KEY_TAB)       MAP_MAC_KEY(49, KEY_SPACE)  /*MAP_MAC_KEY(50, )*/
            MAP_MAC_KEY(51, KEY_BACKSPACE) /*MAP_MAC_KEY(52, )*/       MAP_MAC_KEY(53, KEY_ESC)
            /*MAP_MAC_KEY(54, )*/          /*MAP_MAC_KEY(55, )*/       MAP_MAC_KEY(56, KEY_SHIFT)
            MAP_MAC_KEY(57, KEY_CAPS)      MAP_MAC_KEY(58, KEY_ALT)    MAP_MAC_KEY(59, KEY_CONTROL)

            MAP_MAC_KEY(123, KEY_LEFT)     MAP_MAC_KEY(124, KEY_RIGHT) MAP_MAC_KEY(125, KEY_DOWN)
            MAP_MAC_KEY(126, KEY_UP)

            default: break;
        }
        return false;
    }

    // static
    bool Keyboard::unmapKey(int key, int* raw_key) {
        switch (key) {
            UNMAP_MAC_KEY(0, KEY_A)          UNMAP_MAC_KEY(1, KEY_S)       UNMAP_MAC_KEY(2, KEY_D)
            UNMAP_MAC_KEY(3, KEY_F)          UNMAP_MAC_KEY(4, KEY_H)       UNMAP_MAC_KEY(5, KEY_G)
            UNMAP_MAC_KEY(6, KEY_Z)          UNMAP_MAC_KEY(7, KEY_X)       UNMAP_MAC_KEY(8, KEY_C)
            UNMAP_MAC_KEY(9, KEY_V)          /*UNMAP_MAC_KEY(10, )*/       UNMAP_MAC_KEY(11, KEY_B)
            UNMAP_MAC_KEY(12, KEY_Q)         UNMAP_MAC_KEY(13, KEY_W)      UNMAP_MAC_KEY(14, KEY_E)
            UNMAP_MAC_KEY(15, KEY_R)         UNMAP_MAC_KEY(16, KEY_Y)      UNMAP_MAC_KEY(17, KEY_T)
            UNMAP_MAC_KEY(18, KEY_1)         UNMAP_MAC_KEY(19, KEY_2)      UNMAP_MAC_KEY(20, KEY_3)
            UNMAP_MAC_KEY(21, KEY_4)         UNMAP_MAC_KEY(22, KEY_6)      UNMAP_MAC_KEY(23, KEY_5)
            /*UNMAP_MAC_KEY(24, )*/          UNMAP_MAC_KEY(25, KEY_9)      UNMAP_MAC_KEY(26, KEY_7)
            /*UNMAP_MAC_KEY(27, )*/          UNMAP_MAC_KEY(28, KEY_8)      UNMAP_MAC_KEY(29, KEY_0)
            /*UNMAP_MAC_KEY(30, )*/          UNMAP_MAC_KEY(31, KEY_O)      UNMAP_MAC_KEY(32, KEY_U)
            /*UNMAP_MAC_KEY(33, )*/          UNMAP_MAC_KEY(34, KEY_I)      UNMAP_MAC_KEY(35, KEY_P)
            UNMAP_MAC_KEY(36, KEY_ENTER)     UNMAP_MAC_KEY(37, KEY_L)      UNMAP_MAC_KEY(38, KEY_J)
            /*UNMAP_MAC_KEY(39, )*/          UNMAP_MAC_KEY(40, KEY_K)      /*UNMAP_MAC_KEY(41, )*/
            /*UNMAP_MAC_KEY(42, )*/          /*UNMAP_MAC_KEY(43, )*/       /*UNMAP_MAC_KEY(44, )*/
            UNMAP_MAC_KEY(45, KEY_N)         UNMAP_MAC_KEY(46, KEY_M)      /*UNMAP_MAC_KEY(47, )*/
            UNMAP_MAC_KEY(48, KEY_TAB)       UNMAP_MAC_KEY(49, KEY_SPACE)  /*UNMAP_MAC_KEY(50, )*/
            UNMAP_MAC_KEY(51, KEY_BACKSPACE) /*UNMAP_MAC_KEY(52, )*/       UNMAP_MAC_KEY(53, KEY_ESC)
            /*UNMAP_MAC_KEY(54, )*/          /*UNMAP_MAC_KEY(55, )*/       UNMAP_MAC_KEY(56, KEY_SHIFT)
            UNMAP_MAC_KEY(57, KEY_CAPS)      UNMAP_MAC_KEY(58, KEY_ALT)    UNMAP_MAC_KEY(59, KEY_CONTROL)

            UNMAP_MAC_KEY(123, KEY_LEFT)     UNMAP_MAC_KEY(124, KEY_RIGHT) UNMAP_MAC_KEY(125, KEY_DOWN)
            UNMAP_MAC_KEY(126, KEY_UP)

            default: break;
        }

        return false;
    }

    // static
    bool Keyboard::filterChars(const std::u16string& chars) {
        if (chars.empty()) {
            return true;
        }
        if (chars.size() != 1) {
            return false;
        }

        auto ch = chars[0];
        // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/HandlingKeyEvents/HandlingKeyEvents.html
        if (ch >= 0xF700 && ch <= 0xF8FF) {
            return true;
        }
        // https://developer.apple.com/documentation/appkit/1540619-common_unicode_characters?language=objc
        if (ch == NSEnterCharacter || ch == NSBackspaceCharacter ||
            ch == NSTabCharacter || ch == NSNewlineCharacter ||
            ch == NSFormFeedCharacter || ch == NSCarriageReturnCharacter ||
            ch == NSBackTabCharacter || ch == NSDeleteCharacter ||
            ch == NSLineSeparatorCharacter || ch == NSParagraphSeparatorCharacter ||
            (ch >= 0 && ch <= 31)) // ASCII 控制字符
        {
            return true;
        }

        return false;
    }

    // static
    bool Keyboard::isKeyPressed(int key) {
        int raw_key;
        if (!unmapKey(key, &raw_key)) {
            return false;
        }

        if (CGEventSourceKeyState(
            kCGEventSourceStateCombinedSessionState, (CGKeyCode)raw_key))
        {
            return true;
        }
        return false;
    }

    // static
    bool Keyboard::isKeyToggled(int key) {
        if (key != KEY_CAPS) {
            return false;
        }

        int raw_key;
        if (!unmapKey(key, &raw_key)) {
            return false;
        }

        if (CGEventSourceKeyState(
            kCGEventSourceStateCombinedSessionState, (CGKeyCode)raw_key))
        {
            return true;
        }
        return false;
    }

}
