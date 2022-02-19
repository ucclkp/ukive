// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/input_method_manager.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/text/win/tsf_manager.h"
#elif defined OS_MAC
#include "ukive/text/mac/imm_mac.h"
#endif


namespace ukive {

    InputMethodManager* InputMethodManager::create() {
#ifdef OS_WINDOWS
        return new win::TsfManager();
#elif defined OS_MAC
        return new mac::IMMMac();
#endif
    }

}
