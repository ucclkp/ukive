// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "cyro_buffer.h"

#include "utils/platform_utils.h"

#include "ukive/window/window.h"

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <VersionHelpers.h>

#include "ukive/graphics/win/offscreen_buffer_win.h"
#include "ukive/graphics/win/window_buffer_win7.h"
#include "ukive/graphics/win/window_buffer_win.h"
#include "ukive/window/win/window_impl_win.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/offscreen_buffer_mac.h"
#include "ukive/graphics/mac/window_buffer_mac.h"
#endif


namespace ukive {

    WindowBuffer* WindowBuffer::create(Window* w) {
#ifdef OS_WINDOWS
        if (IsWindows8OrGreater()) {
            return new win::WindowBufferWin(static_cast<win::WindowImplWin*>(w->getImpl()));
        }
        return new win::WindowBufferWin7(static_cast<win::WindowImplWin*>(w->getImpl()));
#elif defined OS_MAC
        return new mac::WindowBufferMac(w);
#endif
    }

    OffscreenBuffer* OffscreenBuffer::create() {
#ifdef OS_WINDOWS
        return new win::OffscreenBufferWin();
#elif defined OS_MAC
        return new mac::OffscreenBufferMac();
#endif
    }

}
