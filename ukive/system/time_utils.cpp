// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/system/time_utils.h"

#include <chrono>

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ukive/system/win/qpc_service.h"
#elif defined OS_MAC
#endif


namespace ukive {

    TimeUtils::TimeUtils() {
    }

    TimeUtils::~TimeUtils() {
    }

    uint64_t TimeUtils::upTimeMillis() {
#ifdef OS_WINDOWS
        return ::GetTickCount64();
#elif defined OS_MAC
        return upTimeMillisPrecise();
#endif
    }

    uint64_t TimeUtils::upTimeMillisPrecise() {
#ifdef OS_WINDOWS
        return QPCService::getTimeStampUs() / 1000;
#elif defined OS_MAC
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
    }

    uint64_t TimeUtils::upTimeMicros() {
#ifdef OS_WINDOWS
        return QPCService::getTimeStampUs();
#elif defined OS_MAC
        return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
    }

}
