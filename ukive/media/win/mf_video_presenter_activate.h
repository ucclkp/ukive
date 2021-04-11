// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_VIDEO_PRESENTER_ACTIVATE_H_
#define UKIVE_MEDIA_WIN_MF_VIDEO_PRESENTER_ACTIVATE_H_

#include <mfidl.h>

#include "ukive/graphics/size.hpp"


namespace ukive {

    class MFVideoPresenter;
    class MFRenderCallback;

    class MFVideoPresenterActivate : public IMFActivate {
    public:
        MFVideoPresenterActivate();
        virtual ~MFVideoPresenterActivate();

        void setDisplaySize(const Size& size);
        void setRenderCallback(MFRenderCallback* cb);

        // IMFActivate
        STDMETHODIMP ActivateObject(const IID& riid, void** ppv) override;
        STDMETHODIMP ShutdownObject() override;
        STDMETHODIMP DetachObject() override;

        // IMFAttributes
        STDMETHODIMP GetItem(const GUID& guidKey, PROPVARIANT* pValue) override;
        STDMETHODIMP GetItemType(const GUID& guidKey, MF_ATTRIBUTE_TYPE* pType) override;
        STDMETHODIMP CompareItem(
            const GUID& guidKey, const PROPVARIANT& Value, BOOL* pbResult) override;
        STDMETHODIMP Compare(
            IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL* pbResult) override;
        STDMETHODIMP GetUINT32(const GUID& guidKey, UINT32* punValue) override;
        STDMETHODIMP GetUINT64(const GUID& guidKey, UINT64* punValue) override;
        STDMETHODIMP GetDouble(const GUID& guidKey, double* pfValue) override;
        STDMETHODIMP GetGUID(const GUID& guidKey, GUID* pguidValue) override;
        STDMETHODIMP GetStringLength(const GUID& guidKey, UINT32* pcchLength) override;
        STDMETHODIMP GetString(
            const GUID& guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32* pcchLength) override;
        STDMETHODIMP GetAllocatedString(
            const GUID& guidKey, LPWSTR* ppwszValue, UINT32* pcchLength) override;
        STDMETHODIMP GetBlobSize(const GUID& guidKey, UINT32* pcbBlobSize) override;
        STDMETHODIMP GetBlob(
            const GUID& guidKey, UINT8* pBuf, UINT32 cbBufSize, UINT32* pcbBlobSize) override;
        STDMETHODIMP GetAllocatedBlob(const GUID& guidKey, UINT8** ppBuf, UINT32* pcbSize) override;
        STDMETHODIMP GetUnknown(const GUID& guidKey, const IID& riid, LPVOID* ppv) override;
        STDMETHODIMP SetItem(const GUID& guidKey, const PROPVARIANT& Value) override;
        STDMETHODIMP DeleteItem(const GUID& guidKey) override;
        STDMETHODIMP DeleteAllItems() override;
        STDMETHODIMP SetUINT32(const GUID& guidKey, UINT32 unValue) override;
        STDMETHODIMP SetUINT64(const GUID& guidKey, UINT64 unValue) override;
        STDMETHODIMP SetDouble(const GUID& guidKey, double fValue) override;
        STDMETHODIMP SetGUID(const GUID& guidKey, const GUID& guidValue) override;
        STDMETHODIMP SetString(const GUID& guidKey, LPCWSTR wszValue) override;
        STDMETHODIMP SetBlob(const GUID& guidKey, const UINT8* pBuf, UINT32 cbBufSize) override;
        STDMETHODIMP SetUnknown(const GUID& guidKey, IUnknown* pUnknown) override;
        STDMETHODIMP LockStore() override;
        STDMETHODIMP UnlockStore() override;
        STDMETHODIMP GetCount(UINT32* pcItems) override;
        STDMETHODIMP GetItemByIndex(UINT32 unIndex, GUID* pguidKey, PROPVARIANT* pValue) override;
        STDMETHODIMP CopyAllItems(IMFAttributes* pDest) override;

        // IUnknown
        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;
        STDMETHODIMP QueryInterface(const IID& riid, void** ppvObject) override;

    private:
        volatile ULONG ref_count_;

        MFVideoPresenter* video_presenter_ = nullptr;

        Size display_size_;
        MFRenderCallback* render_callback_ = nullptr;
    };

}

#endif  // UKIVE_MEDIA_WIN_MF_VIDEO_PRESENTER_ACTIVATE_H_