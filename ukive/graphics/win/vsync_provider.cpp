// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "vsync_provider.h"

#include "utils/message/message.h"

#include "ukive/graphics/display.h"
#include "ukive/graphics/win/display_win.h"
#include "ukive/system/time_utils.h"


namespace ukive {

    VSyncProvider::VSyncProvider() {
        cycler_.setListener(this);
        worker_ = std::thread(std::bind(&VSyncProvider::onWork, this));
    }

    VSyncProvider::~VSyncProvider() {
        need_working_ = false;
        requestVSync();

        if (worker_.joinable()) {
            worker_.join();
        }
    }

    void VSyncProvider::requestVSync() {
        std::unique_lock<std::mutex> ul(cv_mutex_);
        cv_.notify_one();
        cv_pred_ = true;
    }

    void VSyncProvider::setVSyncListener(VSyncListener* l) {
        listener_ = l;
    }

    void VSyncProvider::onHandleMessage(const utl::Message& msg) {
        switch(msg.id) {
        case MSG_VSYNC:
            if (listener_) {
                listener_->onVSync();
            }
            break;

        default:
            break;
        }
    }

    void VSyncProvider::onWork() {
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

            auto before_ts = TimeUtils::upTimeMicros();
            bool ret = static_cast<DisplayWin*>(Display::primary())->waitForVSync();
            auto interval = TimeUtils::upTimeMicros() - before_ts;

            if (!ret || interval < 500) {
                int refresh_rate = Display::primary()->getRefreshRate();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
                interval = TimeUtils::upTimeMicros() - before_ts;
            }

            //DLOG(Log::INFO) << "VSync interval: " << interval;

            cycler_.post(MSG_VSYNC);
        }
    }

}
