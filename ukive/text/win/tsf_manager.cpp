// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "tsf_manager.h"

#include "utils/log.h"


namespace ukive {

    TsfManager::TsfManager()
        : client_id_(0),
          alpn_sink_cookie_(TF_INVALID_COOKIE),
          open_mode_sink_cookie_(TF_INVALID_COOKIE),
          conv_mode_sink_cookie_(TF_INVALID_COOKIE) {
    }

    TsfManager::~TsfManager() {}

    bool TsfManager::initialization() {
        HRESULT hr = ::CoCreateInstance(
            CLSID_TF_ThreadMgr,
            nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(ITfThreadMgr),
            reinterpret_cast<void**>(&thread_mgr_));
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Failed to create thread mgr: " << hr;
            return false;
        }

        hr = thread_mgr_->Activate(&client_id_);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Failed to activate thread mgr: " << hr;
            return false;
        }

        setupSinks();
        return true;
    }

    void TsfManager::destroy() {
        if (thread_mgr_) {
            releaseSinks();
            thread_mgr_->Deactivate();
        }
    }

    bool TsfManager::updateIMEStatus() {
        if (!thread_mgr_) {
            return false;
        }

        ComPtr<ITfCompartmentMgr> cm;
        ComPtr<ITfCompartment> open_mode;
        ComPtr<ITfCompartment> conv_mode;

        if (SUCCEEDED(getCompartments(cm, open_mode, conv_mode))) {
            VARIANT valOpenMode;
            VARIANT valConvMode;
            VariantInit(&valOpenMode);
            VariantInit(&valConvMode);

            HRESULT hr = open_mode->GetValue(&valOpenMode);
            DCHECK(SUCCEEDED(hr));
            hr = conv_mode->GetValue(&valConvMode);
            DCHECK(SUCCEEDED(hr));

            valOpenMode.vt = VT_I4;
            valConvMode.vt = VT_I4;

            valOpenMode.lVal = TRUE;
            valConvMode.lVal = 0;

            hr = open_mode->SetValue(client_id_, &valOpenMode);
            DCHECK(SUCCEEDED(hr));
            hr = conv_mode->SetValue(client_id_, &valConvMode);
            DCHECK(SUCCEEDED(hr));

            VariantClear(&valOpenMode);
            VariantClear(&valConvMode);

            setupCompartmentSinks(open_mode, conv_mode);
        }

        return true;
    }

    TfClientId TsfManager::getClientId() const {
        return client_id_;
    }

    ComPtr<ITfThreadMgr> TsfManager::getThreadManager() const {
        return thread_mgr_;
    }

    HRESULT TsfManager::setupCompartmentSinks(
        const ComPtr<ITfCompartment>& open_mode, const ComPtr<ITfCompartment>& conv_mode)
    {
        ComPtr<ITfSource> src_open_mode;
        ComPtr<ITfSource> src_conv_mode;

        if (!open_mode || !conv_mode) {
            return E_INVALIDARG;
        }

        HRESULT hr = open_mode->QueryInterface(&src_open_mode);
        if (SUCCEEDED(hr)) {
            if (open_mode_sink_cookie_ != TF_INVALID_COOKIE) {
                src_open_mode->UnadviseSink(open_mode_sink_cookie_);
                open_mode_sink_cookie_ = TF_INVALID_COOKIE;
            }

            hr = src_open_mode->AdviseSink(
                IID_ITfCompartmentEventSink,
                static_cast<ITfCompartmentEventSink*>(sink_.get()),
                &open_mode_sink_cookie_);
        }

        hr = conv_mode->QueryInterface(&src_conv_mode);
        if (SUCCEEDED(hr)) {
            if (conv_mode_sink_cookie_ != TF_INVALID_COOKIE) {
                src_conv_mode->UnadviseSink(conv_mode_sink_cookie_);
                conv_mode_sink_cookie_ = TF_INVALID_COOKIE;
            }

            hr = src_conv_mode->AdviseSink(
                IID_ITfCompartmentEventSink,
                static_cast<ITfCompartmentEventSink*>(sink_.get()),
                &conv_mode_sink_cookie_);
        }

        return hr;
    }

    HRESULT TsfManager::releaseCompartmentSinks() {
        ComPtr<ITfCompartmentMgr> cm;
        ComPtr<ITfCompartment> open_mode;
        ComPtr<ITfCompartment> conv_mode;

        HRESULT hr = getCompartments(cm, open_mode, conv_mode);

        ComPtr<ITfSource> srcOpenMode;
        if (SUCCEEDED(hr)) {
            hr = open_mode->QueryInterface(&srcOpenMode);

            if (open_mode_sink_cookie_ != TF_INVALID_COOKIE) {
                srcOpenMode->UnadviseSink(open_mode_sink_cookie_);
                open_mode_sink_cookie_ = TF_INVALID_COOKIE;
            }
        }

        ComPtr<ITfSource> srcConvMode;
        if (SUCCEEDED(hr)) {
            hr = conv_mode->QueryInterface(&srcConvMode);

            if (conv_mode_sink_cookie_ != TF_INVALID_COOKIE) {
                srcConvMode->UnadviseSink(conv_mode_sink_cookie_);
                conv_mode_sink_cookie_ = TF_INVALID_COOKIE;
            }
        }

        return hr;
    }

    HRESULT TsfManager::getCompartments(
        ComPtr<ITfCompartmentMgr>& cm,
        ComPtr<ITfCompartment>& open_mode,
        ComPtr<ITfCompartment>& conv_mode)
    {
        ComPtr<ITfCompartmentMgr> _cm;
        ComPtr<ITfCompartment> _open_mode;
        ComPtr<ITfCompartment> _conv_mode;

        HRESULT hr = thread_mgr_->QueryInterface(&_cm);
        if (FAILED(hr)) {
            return hr;
        }

        hr = _cm->GetCompartment(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE, &_open_mode);
        if (FAILED(hr))
            return hr;

        hr = _cm->GetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &_conv_mode);
        if (FAILED(hr))
            return hr;

        cm = _cm;
        open_mode = _open_mode;
        conv_mode = _conv_mode;
        return S_OK;
    }

    HRESULT TsfManager::setupSinks() {
        HRESULT hr = E_FAIL;

        ComPtr<ITfCompartmentMgr> cm;
        ComPtr<ITfCompartment> open_mode;
        ComPtr<ITfCompartment> conv_mode;

        sink_ = new TsfSink(this);

        auto src = thread_mgr_.cast<ITfSource>();
        if (!src) {
            return E_FAIL;
        }

        hr = src->AdviseSink(
            __uuidof(ITfInputProcessorProfileActivationSink),
            static_cast<ITfInputProcessorProfileActivationSink*>(sink_.get()), &alpn_sink_cookie_);
        if (FAILED(hr)) {
            return hr;
        }

        hr = getCompartments(cm, open_mode, conv_mode);
        if (FAILED(hr)) {
            return hr;
        }

        hr = setupCompartmentSinks(open_mode, conv_mode);
        return hr;
    }

    HRESULT TsfManager::releaseSinks() {
        ComPtr<ITfSource> source;
        HRESULT hr = thread_mgr_->QueryInterface(&source);
        if (FAILED(hr)) {
            return hr;
        }

        if (thread_mgr_) {
            hr = source->UnadviseSink(alpn_sink_cookie_);
            hr = releaseCompartmentSinks();

            alpn_sink_cookie_ = TF_INVALID_COOKIE;
        }

        return hr;
    }


    TsfSink::TsfSink(TsfManager* mgr)
        : ref_count_(1),
          tsf_mgr_(mgr),
          composition_view_(nullptr) {
    }

    TsfSink::~TsfSink() {}

    //切换输入法时响应
    STDMETHODIMP TsfSink::OnActivated(
        DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid, REFGUID guidProfile,
        HKL hkl, DWORD dwFlags)
    {
        switch (dwProfileType) {
        case TF_PROFILETYPE_INPUTPROCESSOR:
            break;
        case TF_PROFILETYPE_KEYBOARDLAYOUT:
            break;
        default:
            break;
        }

        return S_OK;
    }

    //同一输入法中-英切换，日-英切换等等
    STDMETHODIMP TsfSink::OnChange(REFGUID rguid) {
        //DLOG(Log::INFO) << "TsfSink::OnChange";
        return S_OK;
    }

    STDMETHODIMP_(ULONG) TsfSink::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TsfSink::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    STDMETHODIMP TsfSink::QueryInterface(REFIID riid, void **ppvObj) {
        if (!ppvObj) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObj = reinterpret_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITfInputProcessorProfileActivationSink))) {
            *ppvObj = static_cast<ITfInputProcessorProfileActivationSink*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITfCompartmentEventSink))) {
            *ppvObj = static_cast<ITfCompartmentEventSink*>(this);
        } else {
            *ppvObj = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}
