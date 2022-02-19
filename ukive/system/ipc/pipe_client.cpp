// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "pipe_client.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/system/win/ipc/pipe_client_win.h"
#elif defined OS_MAC
#include "ukive/system/mac/ipc/pipe_client_mac.h"
#endif


namespace ukive {

    // static
    PipeClient* PipeClient::create() {
#ifdef OS_WINDOWS
        return new win::PipeClientWin();
#elif defined OS_MAC
        return new mac::PipeClientMac();
#endif
    }

}
