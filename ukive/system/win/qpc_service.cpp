// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "qpc_service.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/log.h"


namespace ukive {

    // QPCService
    thread_local uint64_t QPCService::frequency_ = 0;

    QPCService::QPCService()
        : starting_time_(0) {}

    void QPCService::start() {
        if (!fetchPerformanceFrequency()) {
            return;
        }

        LARGE_INTEGER stime = { 0 };
        BOOL result = ::QueryPerformanceCounter(&stime);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            starting_time_ = 0;
            return;
        }

        starting_time_ = stime.QuadPart;
    }

    uint64_t QPCService::stop() {
        LARGE_INTEGER ending_time = { 0 };

        BOOL result = ::QueryPerformanceCounter(&ending_time);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return 0;
        }

        uint64_t ucount = ending_time.QuadPart - starting_time_;
        uint64_t freq = frequency_;

        uint64_t prev = ucount / freq;
        uint64_t prev_re = ucount % freq;
        uint64_t next = (prev_re * 1000000) / freq;

        return prev * 1000000 + next;
    }

    // static
    uint64_t QPCService::getTimeStampUs() {
        if (!fetchPerformanceFrequency()) {
            return 0;
        }

        LARGE_INTEGER count = { 0 };
        BOOL result = ::QueryPerformanceCounter(&count);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return 0;
        }

        uint64_t ucount = count.QuadPart;
        uint64_t freq = frequency_;

        uint64_t prev = ucount / freq;
        uint64_t prev_re = ucount % freq;
        uint64_t next = (prev_re * 1000000) / freq;

        return prev * 1000000 + next;
    }

    // static
    bool QPCService::fetchPerformanceFrequency() {
        if (frequency_ == 0) {
            LARGE_INTEGER freq = { 0 };
            BOOL result = ::QueryPerformanceFrequency(&freq);
            if (result == 0) {
                LOG(Log::WARNING) << "Failed to get QPF: " << ::GetLastError();
                return false;
            }
            frequency_ = freq.QuadPart;
        }
        return true;
    }

}
