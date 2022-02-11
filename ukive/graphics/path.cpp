// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "path.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/path_win.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/path_mac.h"
#endif


namespace ukive {

    Path* Path::create() {
#ifdef OS_WINDOWS
        return new win::PathWin();
#elif defined OS_MAC
        return new PathMac();
#endif
    }

}
