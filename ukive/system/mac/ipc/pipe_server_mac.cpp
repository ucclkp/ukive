// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "pipe_server_mac.h"

#include "utils/log.h"


namespace ukive {
namespace mac {

    PipeServerMac::PipeServerMac() {
    }

    PipeServerMac::~PipeServerMac() {
    }

    bool PipeServerMac::create(const std::u16string_view& name) {
        return false;
    }

    bool PipeServerMac::wait() {
        return false;
    }

    bool PipeServerMac::read(std::string* buf) {
        return false;
    }

    bool PipeServerMac::write(const std::string_view& buf) {
        return false;
    }

    bool PipeServerMac::disconnect() {
        return true;
    }

    void PipeServerMac::destroy() {
    }

}
}
