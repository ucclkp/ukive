// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "pipe_client_mac.h"

#include "utils/log.h"


namespace ukive {
namespace mac {

    PipeClientMac::PipeClientMac() {}

    PipeClientMac::~PipeClientMac() {
    }

    bool PipeClientMac::connect(const std::u16string_view& name) {
        return false;
    }

    void PipeClientMac::disconnect() {
    }

    bool PipeClientMac::read(std::string* buf) {
        return false;
    }

    bool PipeClientMac::write(const std::string_view& buf) {
        return false;
    }

}
}
