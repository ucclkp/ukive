// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_MAC_IMM_MAC_H_
#define UKIVE_TEXT_MAC_IMM_MAC_H_

#include "ukive/text/input_method_manager.h"


namespace ukive {
namespace mac {

    class IMMMac : public InputMethodManager {
    public:
        IMMMac() = default;

        bool initialization() override;
        void destroy() override;

        bool updateIMEStatus() override;
    };

}
}

#endif  // UKIVE_TEXT_MAC_IMM_MAC_H_
