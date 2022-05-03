// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/mac/input_connection_mac.h"


namespace ukive {
namespace mac {

    InputConnectionMac::InputConnectionMac(TextInputClient* tic) {}

    bool InputConnectionMac::initialize() {
        return true;
    }

    bool InputConnectionMac::pushEditor() {
        return true;
    }

    bool InputConnectionMac::popEditor() {
        return true;
    }

    bool InputConnectionMac::requestFocus() {
        return true;
    }

    bool InputConnectionMac::discardFocus() {
        return true;
    }

    bool InputConnectionMac::terminateComposition() {
        return true;
    }

    bool InputConnectionMac::isCompositing() const {
        return true;
    }

    void InputConnectionMac::notifyStatusChanged(uint32_t flags) {

    }

    void InputConnectionMac::notifyTextChanged(bool correction, const TextChangeInfo &info) {

    }

    void InputConnectionMac::notifyTextLayoutChanged(TextLayoutChangeReason r) {

    }

    void InputConnectionMac::notifyTextSelectionChanged() {

    }

}
}
