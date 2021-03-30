// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_INPUT_METHOD_CONNECTION_H_
#define UKIVE_TEXT_INPUT_METHOD_CONNECTION_H_

#include <cstddef>
#include <cstdint>


namespace ukive {

    class TextInputClient;

    class InputMethodConnection {
    public:
        static InputMethodConnection* create(TextInputClient* tic);

        enum TextLayoutChangeReason {
            TLC_CREATE,
            TLC_CHANGE,
            TLC_DESTROY,
        };

        struct TextChangeInfo {
            size_t start;
            size_t old_end;
            size_t new_end;
        };

        virtual ~InputMethodConnection() = default;

        virtual bool initialization() = 0;

        virtual bool pushEditor() = 0;
        virtual bool popEditor() = 0;

        virtual bool requestFocus() = 0;
        virtual bool discardFocus() = 0;

        virtual bool terminateComposition() = 0;

        virtual bool isCompositing() const = 0;

        virtual void notifyStatusChanged(uint32_t flags) = 0;
        virtual void notifyTextChanged(bool correction, const TextChangeInfo& info) = 0;
        virtual void notifyTextLayoutChanged(TextLayoutChangeReason r) = 0;
        virtual void notifyTextSelectionChanged() = 0;
    };

}

#endif  // UKIVE_TEXT_INPUT_METHOD_CONNECTION_H_
