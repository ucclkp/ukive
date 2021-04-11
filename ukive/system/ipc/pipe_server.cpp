// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "pipe_server.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/system/win/ipc/pipe_server_win.h"
#endif


namespace ukive {

    // static
    PipeServer* PipeServer::create() {
#ifdef OS_WINDOWS
        return new PipeServerWin();
#endif
    }

}
