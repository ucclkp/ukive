// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/window_native.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/window/win/window_impl_win.h"
#elif defined OS_MAC
#include "ukive/window/mac/window_impl_mac.h"
#endif


namespace ukive {

    WindowNative* WindowNative::create(WindowNativeDelegate* w) {
#ifdef OS_WINDOWS
        return new win::WindowImplWin(w);
#elif defined OS_MAC
        return new WindowImplMac(w);
#endif
    }

}
