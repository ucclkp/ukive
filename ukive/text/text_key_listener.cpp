// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "text_key_listener.h"

#include "ukive/event/keyboard.h"
#include "ukive/system/clipboard_manager.h"
#include "ukive/text/editable.h"


namespace ukive {

    TextKeyListener::TextKeyListener() {}

    TextKeyListener::~TextKeyListener() {}

    void TextKeyListener::del(Editable* editable) {
        if (editable->hasSelection()) {
            editable->remove(Editable::Reason::USER_INPUT);
        } else {
            auto start = editable->getSelection().start;
            auto off = editable->getNextOffset(start);
            if (off > 0) {
                editable->remove(start, off, Editable::Reason::USER_INPUT);
                editable->setSelection(start, Editable::Reason::USER_INPUT);
            }
        }
    }

    void TextKeyListener::backspace(Editable* editable) {
        if (editable->hasSelection()) {
            editable->remove(Editable::Reason::USER_INPUT);
        } else {
            auto start = editable->getSelection().start;
            auto off = editable->getPrevOffset(start);
            if (off > 0) {
                editable->remove(start - off, off, Editable::Reason::USER_INPUT);
                editable->setSelection(start - off, Editable::Reason::USER_INPUT);
            }
        }
    }

    void TextKeyListener::newline(Editable* editable) {
        if (editable->hasSelection()) {
            editable->replace(u"\n", Editable::Reason::USER_INPUT);
        } else {
            editable->insert(u"\n", Editable::Reason::USER_INPUT);
        }
    }

    void TextKeyListener::tab(Editable* editable) {
        if (editable->hasSelection()) {
            editable->replace(u"\t", Editable::Reason::USER_INPUT);
        } else {
            editable->insert(u"\t", Editable::Reason::USER_INPUT);
        }
    }

    void TextKeyListener::onChars(
        Editable* editable, bool can_edit, bool can_select, const std::u16string& chars)
    {
        if (!can_edit) {
            return;
        }

        if (Keyboard::isKeyPressed(Keyboard::KEY_CONTROL)) {
            return;
        }

        if (!chars.empty()) {
            if (editable->hasSelection()) {
                editable->replace(chars, Editable::Reason::USER_INPUT);
            } else {
                editable->insert(chars, Editable::Reason::USER_INPUT);
            }
        }
    }

    void TextKeyListener::onKeyDown(
        Editable* editable, bool can_edit, bool can_select, int key)
    {
        if (can_edit) {
            switch (key) {
            case Keyboard::KEY_BACKSPACE:
                backspace(editable);
                return;

            case Keyboard::KEY_DELETE:
                del(editable);
                return;

            case Keyboard::KEY_ENTER:
                newline(editable);
                return;

            case Keyboard::KEY_TAB:
                tab(editable);
                return;

            default:
                break;
            }
        }

        bool is_ctrl_key_pressed = Keyboard::isKeyPressed(Keyboard::KEY_CONTROL);
        if (!is_ctrl_key_pressed) {
            return;
        }

        switch (key) {
        case Keyboard::KEY_A:  //a: select all.
            if (can_select) {
                editable->setSelection(0, editable->length(), Editable::Reason::USER_INPUT);
            }
            break;

        case Keyboard::KEY_Z:  //z: undo.
            if (can_edit) {

            }
            break;

        case Keyboard::KEY_X:  //x: cut.
            if (can_edit && editable->hasSelection()) {
                ClipboardManager::saveToClipboard(editable->getSelectionString());
                editable->remove(Editable::Reason::USER_INPUT);
            }
            break;

        case Keyboard::KEY_C:  //c: copy.
            if (can_edit && editable->hasSelection()) {
                ClipboardManager::saveToClipboard(editable->getSelectionString());
            }
            break;

        case Keyboard::KEY_V:  //v: paste.
            if (can_edit) {
                auto content = ClipboardManager::getFromClipboard();
                if (!content.empty()) {
                    if (editable->hasSelection()) {
                        editable->replace(content, Editable::Reason::USER_INPUT);
                    } else {
                        editable->insert(content, Editable::Reason::USER_INPUT);
                    }
                }
            }
            break;

        default:
            break;
        }
    }

    void TextKeyListener::onKeyUp(
        Editable* editable, bool can_edit, bool can_select, int key)
    {
    }

}