// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_CRITICAL_SECTION_H_
#define UKIVE_SYSTEM_WIN_CRITICAL_SECTION_H_

#include <Windows.h>


namespace ukive::win {

    class CritSec {
    public:
        CritSec();
        explicit CritSec(DWORD spin_count);
        ~CritSec();

        void lock();
        bool tryLock();
        void unlock();

    private:
        CRITICAL_SECTION cs_;
    };


    class CritSecGuard {
    public:
        explicit CritSecGuard(CritSec& cs);
        ~CritSecGuard();

    private:
        CritSec& cs_;
    };

}

#endif  // UKIVE_SYSTEM_WIN_CRITICAL_SECTION_H_