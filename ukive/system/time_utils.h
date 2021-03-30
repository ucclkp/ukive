// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_TIME_UTILS_H_
#define UKIVE_SYSTEM_TIME_UTILS_H_

#include <cstdint>


namespace ukive {

    class TimeUtils {
    public:
        TimeUtils();
        ~TimeUtils();

        static uint64_t upTimeMillis();
        static uint64_t upTimeMillisPrecise();
        static uint64_t upTimeMicros();
    };

}

#endif  // UKIVE_SYSTEM_TIME_UTILS_H_