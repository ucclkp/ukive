// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_CRITICAL_SECTION_HPP_
#define UKIVE_SYSTEM_WIN_CRITICAL_SECTION_HPP_

#include <Windows.h>


namespace ukive {
namespace win {

    class CritSec {
    public:
        CritSec() {
            ::InitializeCriticalSection(&cs_);
        }

        explicit CritSec(DWORD spin_count) {
            ::InitializeCriticalSectionAndSpinCount(&cs_, spin_count);
        }

        ~CritSec() {
            ::DeleteCriticalSection(&cs_);
        }

        void lock() {
            ::EnterCriticalSection(&cs_);
        }

        bool tryLock() {
            return ::TryEnterCriticalSection(&cs_) != 0;
        }

        void unlock() {
            ::LeaveCriticalSection(&cs_);
        }

    private:
        CRITICAL_SECTION cs_;
    };


    class CritSecGuard {
    public:
        explicit CritSecGuard(CritSec& cs)
            : cs_(cs)
        {
            cs_.lock();
        }

        ~CritSecGuard() {
            cs_.unlock();
        }

    private:
        CritSec& cs_;
    };

}
}

#endif  // UKIVE_SYSTEM_WIN_CRITICAL_SECTION_HPP_