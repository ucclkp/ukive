// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_QPC_SERVICE_H_
#define UKIVE_SYSTEM_WIN_QPC_SERVICE_H_

#include <cstdint>

namespace ukive {

    class QPCService {
    public:
        QPCService();
        ~QPCService() = default;

        void start();
        uint64_t stop();

        static uint64_t getTimeStampUs();

    private:
        static bool fetchPerformanceFrequency();

        uint64_t starting_time_;
        static thread_local uint64_t frequency_;
    };

}

#endif  // UKIVE_SYSTEM_WIN_QPC_SERVICE_H_