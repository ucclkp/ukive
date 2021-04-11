// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "mf_sample_scheduler.h"

#include <mfapi.h>
#include <Mferror.h>


namespace {

    enum Event {
        EV_TERMINATE = WM_USER,
        EV_SCHEDULE  = WM_USER + 1,
        EV_FLUSH     = WM_USER + 2,
    };

    class HandleGuard {
    public:
        explicit HandleGuard(HANDLE& handle)
            : handle_(handle) {}
        ~HandleGuard() {
            if (handle_) {
                ::CloseHandle(handle_);
                handle_ = nullptr;
            }
        }

    private:
        HANDLE& handle_;
    };

}

namespace ukive {

    MFSampleScheduler::MFSampleScheduler() {
    }

    MFSampleScheduler::~MFSampleScheduler() {
    }

    void MFSampleScheduler::setCallback(Callback* cb) {
        callback_ = cb;
    }

    void MFSampleScheduler::setFrameRate(const MFRatio& fps) {
        UINT64 avg_time_per_frame = 0;
        ::MFFrameRateToAverageTimePerFrame(fps.Numerator, fps.Denominator, &avg_time_per_frame);

        per_frame_interval_ = MFTIME(avg_time_per_frame);
        per_frame_1_4th_ = per_frame_interval_ / 4;
    }

    void MFSampleScheduler::setClockRate(float rate) {
        rate_ = rate;
    }

    HRESULT MFSampleScheduler::start(const ComPtr<IMFClock>& clock) {
        if (schedule_thread_) {
            return E_UNEXPECTED;
        }

        clock_ = clock;
        thread_ready_event_ = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!thread_ready_event_) {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        HandleGuard guard(thread_ready_event_);

        flush_event_ = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!flush_event_) {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        DWORD tid;
        schedule_thread_ = ::CreateThread(nullptr, 0, scheduleThreadProc, this, 0, &tid);
        if (!schedule_thread_) {
            ::CloseHandle(flush_event_);
            flush_event_ = nullptr;
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        HANDLE objects[]{ thread_ready_event_, schedule_thread_ };
        DWORD wait = ::WaitForMultipleObjects(2, objects, FALSE, INFINITE);
        if (wait != WAIT_OBJECT_0) {
            ::CloseHandle(schedule_thread_);
            schedule_thread_ = nullptr;

            ::CloseHandle(flush_event_);
            flush_event_ = nullptr;
            return E_UNEXPECTED;
        }

        thread_id_ = tid;
        //timeBeginPeriod(1);
        return S_OK;
    }

    void MFSampleScheduler::stop() {
        if (schedule_thread_) {
            ::PostThreadMessageW(thread_id_, EV_TERMINATE, 0, 0);
            ::WaitForSingleObject(schedule_thread_, INFINITE);

            ::CloseHandle(schedule_thread_);
            schedule_thread_ = nullptr;
        }

        if (flush_event_) {
            ::CloseHandle(flush_event_);
            flush_event_ = nullptr;
        }

        list_lock_.lock();
        scheduled_samples_.clear();
        list_lock_.unlock();

        //timeEndPeriod(1);
    }

    HRESULT MFSampleScheduler::scheduleSample(const ComPtr<IMFSample>& sample, bool present_now) {
        if (!callback_) {
            return MF_E_NOT_INITIALIZED;
        }
        if (!schedule_thread_) {
            return MF_E_NOT_INITIALIZED;
        }

        DWORD exit_code = 0;
        ::GetExitCodeThread(schedule_thread_, &exit_code);
        if (exit_code != STILL_ACTIVE) {
            return E_FAIL;
        }

        HRESULT hr = S_OK;
        if (present_now || !clock_) {
            callback_->presentSample(sample.get(), 0);
        } else {
            list_lock_.lock();
            scheduled_samples_.push_back(sample);
            list_lock_.unlock();

            ::PostThreadMessageW(thread_id_, EV_SCHEDULE, 0, 0);
        }

        return hr;
    }

    HRESULT MFSampleScheduler::processSamplesInQueue(LONG* next_sleep) {
        HRESULT hr = S_OK;
        LONG wait = 0;
        for (;;) {
            ComPtr<IMFSample> sample;
            {
                win::CritSecGuard guard(list_lock_);
                if (scheduled_samples_.empty()) {
                    break;
                }
                sample = scheduled_samples_.front();
                scheduled_samples_.pop_front();
            }

            hr = processSample(sample, &wait);
            if (FAILED(hr) || wait > 0) {
                break;
            }
        }

        if (wait == 0) {
            wait = INFINITE;
        }
        *next_sleep = wait;
        return hr;
    }

    HRESULT MFSampleScheduler::processSample(const ComPtr<IMFSample>& sample, LONG* next_sleep) {
        HRESULT hr = S_OK;
        LONGLONG presentation_time = 0;
        LONGLONG time_now = 0;
        LONGLONG system_time = 0;
        bool present_now = true;
        LONG next_sleep_duration = 0;

        if (clock_) {
            hr = sample->GetSampleTime(&presentation_time);
            if (SUCCEEDED(hr)) {
                hr = clock_->GetCorrelatedTime(0, &time_now, &system_time);
            }

            LONGLONG delta = presentation_time - time_now;
            if (rate_ < 0) {
                delta = -delta;
            }

            if (delta < -per_frame_1_4th_) {
                present_now = true;
            } else if (delta > (3 * per_frame_1_4th_)) {
                next_sleep_duration = LONG((delta - per_frame_1_4th_ * 3) / 10000);
                next_sleep_duration = LONG(next_sleep_duration / fabsf(rate_));
                present_now = false;
            }
        }

        if (present_now) {
            hr = callback_->presentSample(sample.get(), presentation_time);
        } else {
            list_lock_.lock();
            scheduled_samples_.push_front(sample);
            list_lock_.unlock();
        }

        *next_sleep = next_sleep_duration;
        return hr;
    }

    void MFSampleScheduler::flush() {
        if (schedule_thread_) {
            ::PostThreadMessageW(thread_id_, EV_FLUSH, 0, 0);
            HANDLE objects[]{ flush_event_, schedule_thread_ };
            ::WaitForMultipleObjects(2, objects, FALSE, 5000);
        }
    }

    LONGLONG MFSampleScheduler::getLastSampleTime() const {
        return last_sample_time_;
    }

    LONGLONG MFSampleScheduler::getFrameDuration() const {
        return per_frame_interval_;
    }

    // static
    DWORD WINAPI MFSampleScheduler::scheduleThreadProc(LPVOID param) {
        auto scheduler = static_cast<MFSampleScheduler*>(param);
        if (!scheduler) {
            return -1;
        }
        return scheduler->scheduleThreadProcPrivate();
    }

    DWORD MFSampleScheduler::scheduleThreadProcPrivate() {
        MSG msg;
        ::PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        ::SetEvent(thread_ready_event_);

        bool finished = false;
        LONG wait = INFINITE;
        HRESULT hr = S_OK;
        while (!finished) {
            DWORD result = ::MsgWaitForMultipleObjects(0, nullptr, FALSE, wait, QS_POSTMESSAGE);
            if (result == WAIT_TIMEOUT) {
                hr = processSamplesInQueue(&wait);
                if (FAILED(hr)) {
                    finished = true;
                }
            }

            while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                bool process_samples = true;

                switch (msg.message) {
                case EV_TERMINATE:
                    finished = true;
                    break;

                case EV_FLUSH:
                    list_lock_.lock();
                    scheduled_samples_.clear();
                    list_lock_.unlock();

                    wait = INFINITE;
                    ::SetEvent(flush_event_);
                    break;

                case EV_SCHEDULE:
                    if (process_samples) {
                        hr = processSamplesInQueue(&wait);
                        if (FAILED(hr)) {
                            finished = true;
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }

        return SUCCEEDED(hr) ? 0 : 1;
    }

}
