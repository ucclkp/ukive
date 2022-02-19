// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/mac/imm_mac.h"


namespace ukive {
namespace mac {

    bool IMMMac::initialization() {
        return true;
    }

    void IMMMac::destroy() {
    }

    bool IMMMac::updateIMEStatus() {
        return false;
    }

}
}
