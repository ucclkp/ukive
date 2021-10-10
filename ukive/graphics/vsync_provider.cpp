// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsync_provider.h"

#include "utils/multi_callbacks.hpp"
#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/vsync_provider_win.h"
#endif


namespace ukive {

    // static
    VSyncProvider* VSyncProvider::create() {
#ifdef OS_WINDOWS
        return new VSyncProviderWin();
#endif
    }

    void VSyncProvider::addCallback(VSyncCallback* cb) {
        utl::addCallbackTo(callbacks_, cb);
    }

    void VSyncProvider::removeCallback(VSyncCallback* cb) {
        utl::removeCallbackFrom(callbacks_, cb);
    }

    bool VSyncProvider::startVSync() {
        if (counter_ <= 0) {
            counter_ = 1;
            return onStartVSync();
        }
        ++counter_;
        return true;
    }

    bool VSyncProvider::stopVSync() {
        if (counter_ <= 0) {
            return false;
        }

        --counter_;
        if (counter_ <= 0) {
            counter_ = 0;
            return onStopVSync();
        }
        return true;
    }

    void VSyncProvider::notifyCallbacks(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        for (auto cb : callbacks_) {
            cb->onVSync(start_time, display_freq, real_interval);
        }
    }

}
