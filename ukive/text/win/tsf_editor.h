// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_TEXT_WIN_TSF_EDITOR_H_
#define UKIVE_TEXT_WIN_TSF_EDITOR_H_

#include <msctf.h>
#include <TextStor.h>

#include <memory>
#include <queue>

#include "utils/memory/win/com_ptr.hpp"


namespace ukive {
namespace win {

    class TsfInputConnection;

    class TsfEditor : public ITextStoreACP, public ITfContextOwnerCompositionSink
    {
    public:
        explicit TsfEditor(TsViewCookie tvc);
        virtual ~TsfEditor();

        void setInputConnection(TsfInputConnection* ic);

        void notifyTextChanged(bool correction, LONG start, LONG oldEnd, LONG newEnd);
        void notifyTextLayoutChanged(TsLayoutCode reason);
        void notifyTextSelectionChanged();
        void notifyStatusChanged(DWORD flags);
        void notifyAttrsChanged(LONG start, LONG end, ULONG attrsCount, const TS_ATTRID* attrs);

        bool isCompositing() const;
        bool hasReadOnlyLock() const;
        bool hasReadWriteLock() const;

        //ITextStoreACP methods.
        STDMETHODIMP AdviseSink(REFIID riid, IUnknown* punk, DWORD dwMask) override;
        STDMETHODIMP UnadviseSink(IUnknown* punk) override;
        STDMETHODIMP RequestLock(DWORD dwLockFlags, HRESULT* phrSession) override;

        STDMETHODIMP GetStatus(TS_STATUS* pdcs) override;
        STDMETHODIMP QueryInsert(
            LONG acpTestStart, LONG acpTestEnd, ULONG cch,
            LONG* pacpResultStart, LONG* pacpResultEnd) override;
        STDMETHODIMP GetSelection(
            ULONG ulIndex, ULONG ulCount,
            TS_SELECTION_ACP* pSelection, ULONG* pcFetched) override;
        STDMETHODIMP SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) override;
        STDMETHODIMP GetText(
            LONG acpStart, LONG acpEnd,
            WCHAR* pchPlain, ULONG cchPlainReq, ULONG* pcchPlainRet,
            TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfoRet, LONG* pacpNext) override;
        STDMETHODIMP SetText(
            DWORD dwFlags, LONG acpStart, LONG acpEnd,
            const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange) override;
        STDMETHODIMP GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject* *ppDataObject) override;
        STDMETHODIMP GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown* *ppunk) override;
        STDMETHODIMP QueryInsertEmbedded(const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable) override;
        STDMETHODIMP InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange) override;
        STDMETHODIMP RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs) override;
        STDMETHODIMP RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) override;
        STDMETHODIMP RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) override;
        STDMETHODIMP FindNextAttrTransition(
            LONG acpStart, LONG acpHalt,
            ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs,
            DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset) override;
        STDMETHODIMP RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL* paAttrVals, ULONG* pcFetched) override;
        STDMETHODIMP GetEndACP(LONG* pacp) override;
        STDMETHODIMP GetActiveView(TsViewCookie* pvcView) override;
        STDMETHODIMP GetACPFromPoint(TsViewCookie vcView, const POINT* pt, DWORD dwFlags, LONG* pacp) override;
        STDMETHODIMP GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) override;
        STDMETHODIMP GetScreenExt(TsViewCookie vcView, RECT* prc) override;
        STDMETHODIMP GetWnd(TsViewCookie vcView, HWND* phwnd) override;
        STDMETHODIMP InsertTextAtSelection(
            DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) override;
        STDMETHODIMP InsertEmbeddedAtSelection(
            DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) override;

        //ITfContextOwnerCompositionSink methods.
        STDMETHODIMP OnStartComposition(
            ITfCompositionView* pComposition,
            BOOL* pfOk) override;

        STDMETHODIMP OnUpdateComposition(
            ITfCompositionView* pComposition,
            ITfRange* pRangeNew) override;

        STDMETHODIMP OnEndComposition(
            ITfCompositionView* pComposition) override;

        //IUnknown methods.
        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;
        STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override;

    private:
        struct SinkRecord {
            utl::win::ComPtr<IUnknown> punk_id;
            utl::win::ComPtr<ITextStoreACPSink> sink;
            DWORD mask;
        };

        struct LockRecord {
            DWORD lock_flags;
        };

        ULONG ref_count_;

        bool has_lock_;
        DWORD cur_lock_type_;
        SinkRecord sink_record_;
        TsfInputConnection* input_conn_;
        std::queue<std::shared_ptr<LockRecord>> req_queue_;

        bool is_compositing_ = false;
        TsViewCookie view_cookie_;
    };

}
}

#endif  // UKIVE_TEXT_WIN_TSF_EDITOR_H_