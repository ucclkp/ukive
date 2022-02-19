// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_VSYNC_PROVIDER_MAC_H_
#define UKIVE_GRAPHICS_MAC_VSYNC_PROVIDER_MAC_H_

#include <atomic>
#include <condition_variable>
#include <thread>

#include "utils/message/cycler.h"

#include "ukive/graphics/vsync_provider.h"

#include <CoreVideo/CVDisplayLink.h>


namespace ukive {
namespace mac {

    /**
     * Windows 上的垂直同步信号提供器。
     * 在 Windows 上 DWM 将垂直同步信号限定为主显示器的垂直同步信号。
     */
    class VSyncProviderMac :
        public VSyncProvider,
        public utl::CyclerListener
    {
    public:
        VSyncProviderMac();
        ~VSyncProviderMac();
        
    protected:
        bool onStartVSync() override;
        bool onStopVSync() override;

        enum MsgType {
            MSG_VSYNC = 0,
        };

        // CyclerListener
        void onHandleMessage(const utl::Message& msg) override;

    private:
        static CVReturn onDisplayLinkCallback(
            CVDisplayLinkRef displayLink,
            const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime,
            CVOptionFlags flagsIn, CVOptionFlags *flagsOut,
            void *displayLinkContext);

        uint32_t refresh_rate_ = 0;
        uint64_t req_time_ = 0;
        utl::Cycler cycler_;
        CVDisplayLinkRef display_link_ = nullptr;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_VSYNC_PROVIDER_MAC_H_
