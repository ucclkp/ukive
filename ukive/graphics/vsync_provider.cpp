// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsync_provider.h"

#include "utils/message/message.h"
#include "utils/multi_callbacks.hpp"
#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/vsync_provider_win.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/vsync_provider_mac.h"
#endif


namespace ukive {

    // static
    VSyncProvider* VSyncProvider::create() {
#ifdef OS_WINDOWS
        return new win::VSyncProviderWin();
#elif defined OS_MAC
        return new mac::VSyncProviderMac();
#endif
    }

    VSyncProvider::VSyncProvider()
        : lagged_(0)
    {
        cycler_.setListener(this);
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

    void VSyncProvider::onHandleMessage(const utl::Message& msg) {
        switch (msg.id) {
        case MSG_VSYNC:
        {
#ifdef NDEBUG
            lagged_.store(0, std::memory_order_relaxed);
#else
            auto val = lagged_.exchange(0, std::memory_order_relaxed);
            if (val > 1) {
                jour_dw("*** VSync lagged! Count: %d", val - 1);
            }
#endif
            if (isRunning()) {
                notifyCallbacks(msg.ui1, uint32_t(msg.ui2 >> 32), uint32_t(msg.ui2));
            }
            break;
        }

        default:
            break;
        }
    }

    void VSyncProvider::sendVSyncToUI(
        uint64_t after_ts, uint32_t refresh_rate, uint32_t real_interval)
    {
        auto val = lagged_.fetch_add(1, std::memory_order_relaxed);
        if (val > 0) {
            return;
        }

        utl::Message msg;
        msg.id = MSG_VSYNC;
        msg.ui1 = after_ts;
        msg.ui2 = (uint64_t(refresh_rate) << 32) | uint32_t(real_interval);
        cycler_.post(&msg);
    }

    void VSyncProvider::notifyCallbacks(
        uint64_t start_time, uint32_t display_freq, uint32_t real_interval)
    {
        for (auto cb : callbacks_) {
            cb->onVSync(start_time, display_freq, real_interval);
        }
    }

}
