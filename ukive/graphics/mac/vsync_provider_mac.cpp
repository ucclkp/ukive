// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsync_provider_mac.h"

#include "utils/log.h"
#include "utils/message/message.h"
#include "utils/time_utils.h"

#include "ukive/graphics/display.h"
#include "ukive/graphics/mac/display_mac.h"


namespace ukive {
namespace mac {

    VSyncProviderMac::VSyncProviderMac() {
        cycler_.setListener(this);

        auto display_id = CGMainDisplayID();

        /**
         * TODO：需要测试 E-Ink 屏幕的行为。刷新率可能是 0 ？
         */
        auto mode = CGDisplayCopyDisplayMode(display_id);
        if (mode) {
            refresh_rate_ = std::ceil(CGDisplayModeGetRefreshRate(mode));
            CGDisplayModeRelease(mode);
        }

        CVReturn ret = CVDisplayLinkCreateWithCGDisplay(display_id, &display_link_);
        if (ret != kCVReturnSuccess) {
            LOG(Log::ERR) << "Cannot create display link: " << ret;
            display_link_ = nullptr;
            return;
        }

        CVDisplayLinkSetOutputCallback(display_link_, onDisplayLinkCallback, this);
    }

    VSyncProviderMac::~VSyncProviderMac() {
        if (display_link_) {
            if (CVDisplayLinkIsRunning(display_link_)) {
                CVDisplayLinkStop(display_link_);
            }

            CVDisplayLinkRelease(display_link_);
            display_link_ = nullptr;
        }
    }

    bool VSyncProviderMac::onStartVSync() {
        if (!display_link_) {
            return false;
        }

        req_time_ = utl::TimeUtils::upTimeNanos();
        if (!CVDisplayLinkIsRunning(display_link_)) {
            CVDisplayLinkStart(display_link_);
        }
        return true;
    }

    bool VSyncProviderMac::onStopVSync() {
        if (!display_link_) {
            return false;
        }

        if (CVDisplayLinkIsRunning(display_link_)) {
            CVDisplayLinkStop(display_link_);
        }
        return true;
    }

    void VSyncProviderMac::onHandleMessage(const utl::Message& msg) {
        switch(msg.id) {
        case MSG_VSYNC:
        {
            if (display_link_ && CVDisplayLinkIsRunning(display_link_)) {
                notifyCallbacks(msg.ui1, uint32_t(msg.ui2 >> 32), uint32_t(msg.ui2));
            }
            break;
        }

        default:
            break;
        }
    }

    // static
    CVReturn VSyncProviderMac::onDisplayLinkCallback(
        CVDisplayLinkRef displayLink,
        const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime,
        CVOptionFlags flagsIn, CVOptionFlags *flagsOut,
        void *displayLinkContext)
    {
        auto This = static_cast<VSyncProviderMac*>(displayLinkContext);

        auto cur_time = utl::TimeUtils::upTimeNanos();
        auto real_interval = cur_time - This->req_time_;
        auto refresh_rate = This->refresh_rate_ > 0 ? This->refresh_rate_ : 60;

        utl::Message msg;
        msg.id = MSG_VSYNC;
        msg.ui1 = cur_time;
        msg.ui2 = (uint64_t(refresh_rate) << 32) | uint32_t(real_interval);
        This->cycler_.post(&msg);

        return kCVReturnSuccess;
    }

}
}
