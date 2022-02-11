// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_SAMPLE_SCHEDULER_H_
#define UKIVE_MEDIA_WIN_MF_SAMPLE_SCHEDULER_H_

#include <mfidl.h>

#include "ukive/media/win/mf_common.h"
#include "ukive/system/win/critical_section.hpp"


namespace ukive {
namespace win {

    class MFSampleScheduler {
    public:
        class Callback {
        public:
            virtual ~Callback() = default;

            virtual HRESULT presentSample(IMFSample* sample, LONGLONG target) = 0;
        };

        MFSampleScheduler();
        ~MFSampleScheduler();

        void setCallback(Callback* cb);
        void setFrameRate(const MFRatio& fps);
        void setClockRate(float rate);

        HRESULT start(const ComPtr<IMFClock>& clock);
        void stop();

        HRESULT scheduleSample(const ComPtr<IMFSample>& sample, bool present_now);
        HRESULT processSamplesInQueue(LONG* next_sleep);
        HRESULT processSample(const ComPtr<IMFSample>& sample, LONG* next_sleep);
        void flush();

        LONGLONG getLastSampleTime() const;
        LONGLONG getFrameDuration() const;

    private:
        static DWORD WINAPI scheduleThreadProc(LPVOID param);
        DWORD scheduleThreadProcPrivate();

        CritSec list_lock_;
        SampleList scheduled_samples_;
        ComPtr<IMFClock> clock_;
        Callback* callback_ = nullptr;

        DWORD thread_id_ = 0;
        HANDLE schedule_thread_ = nullptr;
        HANDLE thread_ready_event_ = nullptr;
        HANDLE flush_event_ = nullptr;

        float rate_ = 1;
        MFTIME per_frame_interval_ = 0;
        LONGLONG per_frame_1_4th_ = 0;
        MFTIME last_sample_time_ = 0;
    };

}
}

#endif  // UKIVE_MEDIA_WIN_MF_SAMPLE_SCHEDULER_H_