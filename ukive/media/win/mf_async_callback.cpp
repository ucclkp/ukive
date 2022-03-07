// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "mf_async_callback.h"


namespace ukive {
namespace win {

    MFAsyncCallback::MFAsyncCallback(const Callback& cb)
        : ref_count_(1),
          callback_(cb)
    {
        event_ = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
    }

    MFAsyncCallback::~MFAsyncCallback() {
        ::CloseHandle(event_);
    }

    void MFAsyncCallback::reset() {
        ::ResetEvent(event_);
        is_finished_ = false;
    }

    void MFAsyncCallback::wait() {
        ::WaitForSingleObject(event_, INFINITE);
        reset();
    }

    bool MFAsyncCallback::isFinished() const {
        return is_finished_;
    }

    STDMETHODIMP MFAsyncCallback::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFAsyncCallback::Invoke(IMFAsyncResult* pAsyncResult) {
        ::SetEvent(event_);
        is_finished_ = true;

        HRESULT hr = S_OK;
        if (callback_) {
            hr = callback_(pAsyncResult);
        }
        return hr;
    }

    STDMETHODIMP_(ULONG) MFAsyncCallback::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) MFAsyncCallback::Release() {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    STDMETHODIMP MFAsyncCallback::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObject = static_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFAsyncCallback))) {
            *ppvObject = static_cast<IMFAsyncCallback*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}
}