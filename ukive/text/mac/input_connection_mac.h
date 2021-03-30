// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_MAC_INPUT_CONNECTION_MAC_H_
#define UKIVE_TEXT_MAC_INPUT_CONNECTION_MAC_H_

#include "ukive/text/input_method_connection.h"


namespace ukive {

    class InputConnectionMac : public InputMethodConnection {
    public:
        explicit InputConnectionMac(TextInputClient* tic);

        bool initialization() override;

        bool pushEditor() override;
        bool popEditor() override;

        bool requestFocus() override;
        bool discardFocus() override;

        bool terminateComposition() override;

        bool isCompositing() const override;

        void notifyStatusChanged(uint32_t flags) override;
        void notifyTextChanged(bool correction, const TextChangeInfo &info) override;
        void notifyTextLayoutChanged(TextLayoutChangeReason r) override;
        void notifyTextSelectionChanged() override;
    };

}

#endif  // UKIVE_TEXT_MAC_INPUT_CONNECTION_MAC_H_
