// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsync_provider_win.h"

#include "utils/log.h"
#include "utils/message/message.h"
#include "utils/time_utils.h"

#include "ukive/graphics/display.h"
#include "ukive/graphics/win/display_win.h"


namespace ukive {

    VSyncProviderWin::VSyncProviderWin() {
        cycler_.setListener(this);
        worker_ = std::thread(std::bind(&VSyncProviderWin::onWork, this));
    }

    VSyncProviderWin::~VSyncProviderWin() {
        need_working_ = false;
        run();

        if (worker_.joinable()) {
            worker_.join();
        }
    }

    void VSyncProviderWin::requestVSync() {
        run();
    }

    void VSyncProviderWin::setPrimaryMonitorStatus(bool opened) {
        pm_opened_.store(opened, std::memory_order_relaxed);
    }

    void VSyncProviderWin::onHandleMessage(const utl::Message& msg) {
        switch(msg.id) {
        case MSG_VSYNC:
            notifyCallbacks(msg.ui1, uint32_t(msg.ui2 >> 32), uint32_t(msg.ui2));
            break;

        default:
            break;
        }
    }

    void VSyncProviderWin::run() {
        std::unique_lock<std::mutex> ul(cv_mutex_);
        cv_.notify_one();
        cv_pred_ = true;
    }

    void VSyncProviderWin::onWork() {
        for (;;) {
            if (!need_working_) {
                break;
            }

            {
                std::unique_lock<std::mutex> ul(cv_mutex_);
                while (!cv_pred_) {
                    cv_.wait(ul);
                }
                cv_pred_ = false;
            }

            if (!need_working_) {
                break;
            }

            auto primary = static_cast<DisplayWin*>(Display::primary());

            auto before_ts = utl::TimeUtils::upTimeNanos();
            bool ret = primary->waitForVSync();
            auto after_ts = utl::TimeUtils::upTimeNanos();

            auto real_interval = after_ts - before_ts;
            auto refresh_rate = primary->getRefreshRate();
            if (refresh_rate == 0) {
                refresh_rate = 1;
            }

            /**
             * 即使显示器关闭，waitForVSync 仍会返回成功，因此需要外部告知显示器状态。
             * 外部告知的时机稍晚于实际显示器关闭的时机（慢1.5秒左右），但已足够使用。如果
             * 显示器已关闭，就根据显示器刷新率算出的帧间隔进行 Sleep。
             * TODO：需要测试 E-Ink 屏幕的行为。刷新率可能是 0 ？
             */
            if (!ret || !pm_opened_.load(std::memory_order_relaxed)) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000 / refresh_rate));
                after_ts = utl::TimeUtils::upTimeNanos();
                real_interval = after_ts - before_ts;
                //DLOG(Log::INFO) << "VSync fallback. interval: " << real_interval << "us";
            } else {
                //DLOG(Log::INFO) << "VSync normal. interval: " << real_interval << "us";
            }

            utl::Message msg;
            msg.id = MSG_VSYNC;
            msg.ui1 = after_ts;
            msg.ui2 = (uint64_t(refresh_rate) << 32) | uint32_t(real_interval);
            cycler_.post(&msg);
        }
    }

}
