// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/input_method_connection.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/text/win/tsf_input_connection.h"
#elif defined OS_MAC
#include "ukive/text/mac/input_connection_mac.h"
#endif


namespace ukive {

    InputMethodConnection* InputMethodConnection::create(TextInputClient* tic) {
#ifdef OS_WINDOWS
        return new win::TsfInputConnection(tic);
#elif defined OS_MAC
        return new mac::InputConnectionMac(tic);
#endif
    }

}
