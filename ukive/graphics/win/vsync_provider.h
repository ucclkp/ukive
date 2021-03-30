// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_VSYNC_PROVIDER_H_
#define UKIVE_GRAPHICS_VSYNC_PROVIDER_H_

#include <condition_variable>
#include <thread>

#include "utils/message/cycler.h"


namespace ukive {

    class VSyncListener {
    public:
        virtual ~VSyncListener() = default;

        virtual void onVSync() = 0;
    };


    class VSyncProvider : public utl::CyclerListener {
    public:
        VSyncProvider();
        ~VSyncProvider();

        void requestVSync();
        void setVSyncListener(VSyncListener* l);

    protected:
        enum MsgType {
            MSG_VSYNC = 0,
        };

        // CyclerListener
        void onHandleMessage(const utl::Message& msg) override;

    private:
        void onWork();

        utl::Cycler cycler_;
        std::thread worker_;
        std::mutex cv_mutex_;
        std::condition_variable cv_;
        bool cv_pred_ = false;
        bool need_working_ = true;
        VSyncListener* listener_ = nullptr;
    };

}

#endif  // UKIVE_GRAPHICS_VSYNC_PROVIDER_H_