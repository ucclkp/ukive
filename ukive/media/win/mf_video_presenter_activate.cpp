// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "mf_video_presenter_activate.h"

#include "ukive/media/win/mf_video_presenter.h"


namespace ukive {

    MFVideoPresenterActivate::MFVideoPresenterActivate()
        : ref_count_(1) {}

    MFVideoPresenterActivate::~MFVideoPresenterActivate() {
        if (video_presenter_) {
            video_presenter_->destroy();
            video_presenter_->Release();
        }
    }

    void MFVideoPresenterActivate::setDisplaySize(const Size& size) {
        display_size_ = size;
        if (video_presenter_) {
            video_presenter_->setDisplaySize(size);
        }
    }

    void MFVideoPresenterActivate::setRenderCallback(MFRenderCallback* cb) {
        render_callback_ = cb;
        if (video_presenter_) {
            video_presenter_->setRenderCallback(cb);
        }
    }

    STDMETHODIMP MFVideoPresenterActivate::ActivateObject(const IID& riid, void** ppv) {
        if (riid == __uuidof(IMFVideoPresenter)) {
            if (!video_presenter_) {
                video_presenter_ = new MFVideoPresenter();
                HRESULT hr = video_presenter_->initialize();
                if (FAILED(hr)) {
                    return hr;
                }
                video_presenter_->setDisplaySize(display_size_);
                video_presenter_->setRenderCallback(render_callback_);
                video_presenter_->AddRef();
            }

            *ppv = video_presenter_;
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP MFVideoPresenterActivate::ShutdownObject() {
        if (!video_presenter_) {
            return S_OK;
        }

        video_presenter_->destroy();
        video_presenter_->Release();
        video_presenter_ = nullptr;

        return S_OK;
    }

    STDMETHODIMP MFVideoPresenterActivate::DetachObject() {
        video_presenter_->Release();
        video_presenter_ = nullptr;

        return S_OK;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetItem(const GUID& guidKey, PROPVARIANT* pValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetItemType(const GUID& guidKey, MF_ATTRIBUTE_TYPE* pType) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::CompareItem(
        const GUID& guidKey, const PROPVARIANT& Value, BOOL* pbResult)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::Compare(
        IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL* pbResult)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetUINT32(const GUID& guidKey, UINT32* punValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetUINT64(const GUID& guidKey, UINT64* punValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetDouble(const GUID& guidKey, double* pfValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetGUID(const GUID& guidKey, GUID* pguidValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetStringLength(const GUID& guidKey, UINT32* pcchLength) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetString(
        const GUID& guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32* pcchLength)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetAllocatedString(
        const GUID& guidKey, LPWSTR* ppwszValue, UINT32* pcchLength)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetBlobSize(const GUID& guidKey, UINT32* pcbBlobSize) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetBlob(
        const GUID& guidKey, UINT8* pBuf, UINT32 cbBufSize, UINT32* pcbBlobSize)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetAllocatedBlob(const GUID& guidKey, UINT8** ppBuf, UINT32* pcbSize) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetUnknown(const GUID& guidKey, const IID& riid, LPVOID* ppv) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetItem(const GUID& guidKey, const PROPVARIANT& Value) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::DeleteItem(const GUID& guidKey) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::DeleteAllItems() {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetUINT32(const GUID& guidKey, UINT32 unValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetUINT64(const GUID& guidKey, UINT64 unValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetDouble(const GUID& guidKey, double fValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetGUID(const GUID& guidKey, const GUID& guidValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetString(const GUID& guidKey, LPCWSTR wszValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetBlob(const GUID& guidKey, const UINT8* pBuf, UINT32 cbBufSize) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::SetUnknown(const GUID& guidKey, IUnknown* pUnknown) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::LockStore() {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::UnlockStore() {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetCount(UINT32* pcItems) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::GetItemByIndex(UINT32 unIndex, GUID* pguidKey, PROPVARIANT* pValue) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenterActivate::CopyAllItems(IMFAttributes* pDest) {
        return E_NOTIMPL;
    }

    STDMETHODIMP_(ULONG) MFVideoPresenterActivate::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) MFVideoPresenterActivate::Release() {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    STDMETHODIMP MFVideoPresenterActivate::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObject = reinterpret_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFAttributes))) {
            *ppvObject = static_cast<IMFAttributes*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFActivate))) {
            *ppvObject = static_cast<IMFActivate*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}
