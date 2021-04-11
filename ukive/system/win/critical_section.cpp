// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "critical_section.h"


namespace ukive::win {

    // CritSec
    CritSec::CritSec() {
        ::InitializeCriticalSection(&cs_);
    }

    CritSec::CritSec(DWORD spin_count) {
        ::InitializeCriticalSectionAndSpinCount(&cs_, spin_count);
    }

    CritSec::~CritSec() {
        ::DeleteCriticalSection(&cs_);
    }

    void CritSec::lock() {
        ::EnterCriticalSection(&cs_);
    }

    bool CritSec::tryLock() {
        return ::TryEnterCriticalSection(&cs_) != 0;
    }

    void CritSec::unlock() {
        ::LeaveCriticalSection(&cs_);
    }


    // CritSecGuard
    CritSecGuard::CritSecGuard(CritSec& cs)
        : cs_(cs)
    {
        cs_.lock();
    }

    CritSecGuard::~CritSecGuard() {
        cs_.unlock();
    }

}
