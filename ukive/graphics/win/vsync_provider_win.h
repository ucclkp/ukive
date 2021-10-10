// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_VSYNC_PROVIDER_WIN_H_
#define UKIVE_GRAPHICS_WIN_VSYNC_PROVIDER_WIN_H_

#include <atomic>
#include <condition_variable>
#include <thread>

#include "utils/message/cycler.h"

#include "ukive/graphics/vsync_provider.h"


namespace ukive {

    /**
     * Windows 上的垂直同步信号提供器。
     * 在 Windows 上 DWM 将垂直同步信号限定为主显示器的垂直同步信号。
     */
    class VSyncProviderWin :
        public VSyncProvider,
        public utl::CyclerListener
    {
    public:
        VSyncProviderWin();
        ~VSyncProviderWin();

        void setPrimaryMonitorStatus(bool opened);

    protected:
        enum MsgType {
            MSG_VSYNC = 0,
        };

        // VSyncProvider
        bool onStartVSync() override;
        bool onStopVSync() override;

        // CyclerListener
        void onHandleMessage(const utl::Message& msg) override;

    private:
        void wake();
        void wait();
        void onWork();

        utl::Cycler cycler_;
        std::thread worker_;
        std::mutex cv_mutex_;
        std::condition_variable cv_;
        bool cv_pred_ = false;
        std::atomic_bool is_finished_;
        std::atomic_bool is_running_;
        std::atomic_bool pm_opened_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_VSYNC_PROVIDER_WIN_H_