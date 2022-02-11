// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_ASYNC_CALLBACK_H_
#define UKIVE_MEDIA_WIN_MF_ASYNC_CALLBACK_H_

#include <functional>

#include <mfidl.h>


namespace ukive {
namespace win {

    class MFAsyncCallback : public IMFAsyncCallback {
    public:
        using Callback = std::function<HRESULT(IMFAsyncResult* result)>;

        explicit MFAsyncCallback(const Callback& cb);
        virtual ~MFAsyncCallback();

        void reset();
        void wait();
        bool isFinished() const;

        // IMFAsyncCallback
        STDMETHODIMP GetParameters(DWORD* pdwFlags, DWORD* pdwQueue) override;
        STDMETHODIMP Invoke(IMFAsyncResult* pAsyncResult) override;

        // IUnknown
        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;
        STDMETHODIMP QueryInterface(const IID& riid, void** ppvObject) override;

    private:
        ULONG ref_count_;
        Callback callback_;
        HANDLE event_;
        bool is_finished_ = false;
    };

}
}

#endif  // UKIVE_MEDIA_WIN_MF_ASYNC_CALLBACK_H_