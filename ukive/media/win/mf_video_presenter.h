// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_VIDEO_PRESENTER_H_
#define UKIVE_MEDIA_WIN_MF_VIDEO_PRESENTER_H_

#include <evr.h>

#include "ukive/graphics/rect.hpp"
#include "ukive/media/win/mf_common.h"
#include "ukive/media/win/mf_sample_recycler.h"
#include "ukive/media/win/mf_sample_scheduler.h"
#include "ukive/system/win/com_ptr.hpp"
#include "ukive/system/win/critical_section.h"


namespace ukive {

    class MFRenderCallback;
    class MFD3D9RenderEngine;

    /**
     * 根据 https://docs.microsoft.com/en-us/windows/win32/medfound/how-to-write-an-evr-presenter
     * 所述内容实现。
     */

    class MFVideoPresenter :
        public IMFVideoPresenter,
        public IMFGetService,
        public IMFTopologyServiceLookupClient,
        public IMFVideoDeviceID,
        public IMFVideoDisplayControl
    {
    public:
        enum class RenderState {
            Started,
            Stopped,
            Paused,
            Shutdown,
        };

        enum class FrameStepState {
            None,
            Waiting,
            Pending,
            Scheduled,
            Complete,
        };

        MFVideoPresenter();
        virtual ~MFVideoPresenter();

        HRESULT initialize();
        void destroy();

        void setDisplaySize(const Size& size);
        void setRenderCallback(MFRenderCallback* cb);

        // IMFClockStateSink
        STDMETHODIMP OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) override;
        STDMETHODIMP OnClockStop(MFTIME hnsSystemTime) override;
        STDMETHODIMP OnClockPause(MFTIME hnsSystemTime) override;
        STDMETHODIMP OnClockRestart(MFTIME hnsSystemTime) override;
        STDMETHODIMP OnClockSetRate(MFTIME hnsSystemTime, float flRate) override;

        // IMFVideoPresenter
        STDMETHODIMP ProcessMessage(MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam) override;
        STDMETHODIMP GetCurrentMediaType(IMFVideoMediaType** ppMediaType) override;

        // IMFGetService
        STDMETHODIMP GetService(
            const GUID& guidService, const IID& riid, LPVOID* ppvObject) override;

        // IMFTopologyServiceLookupClient
        STDMETHODIMP InitServicePointers(IMFTopologyServiceLookup* pLookup) override;
        STDMETHODIMP ReleaseServicePointers() override;

        // IMFVideoDeviceID
        STDMETHODIMP GetDeviceID(IID* pDeviceID) override;

        // IMFVideoDisplayControl
        STDMETHODIMP GetNativeVideoSize(SIZE* pszVideo, SIZE* pszARVideo) override;
        STDMETHODIMP GetIdealVideoSize(SIZE* pszMin, SIZE* pszMax) override;
        STDMETHODIMP SetVideoPosition(const MFVideoNormalizedRect* pnrcSource, const LPRECT prcDest) override;
        STDMETHODIMP GetVideoPosition(MFVideoNormalizedRect* pnrcSource, LPRECT prcDest) override;
        STDMETHODIMP SetAspectRatioMode(DWORD dwAspectRatioMode) override;
        STDMETHODIMP GetAspectRatioMode(DWORD* pdwAspectRatioMode) override;
        STDMETHODIMP SetVideoWindow(HWND hwndVideo) override;
        STDMETHODIMP GetVideoWindow(HWND* phwndVideo) override;
        STDMETHODIMP RepaintVideo() override;
        STDMETHODIMP GetCurrentImage(BITMAPINFOHEADER* pBih, BYTE** pDib, DWORD* pcbDib, LONGLONG* pTimeStamp) override;
        STDMETHODIMP SetBorderColor(COLORREF Clr) override;
        STDMETHODIMP GetBorderColor(COLORREF* pClr) override;
        STDMETHODIMP SetRenderingPrefs(DWORD dwRenderFlags) override;
        STDMETHODIMP GetRenderingPrefs(DWORD* pdwRenderFlags) override;
        STDMETHODIMP SetFullscreen(BOOL fFullscreen) override;
        STDMETHODIMP GetFullscreen(BOOL* pfFullscreen) override;

        // IUnknown
        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;
        STDMETHODIMP QueryInterface(const IID& riid, void** ppvObject) override;

    private:
        struct FrameStep {
            FrameStepState state = FrameStepState::None;
            SampleList samples;
            DWORD steps = 0;
            DWORD_PTR sample_no_ref = 0;
        };

        bool isActive() const;
        bool isScrubbing() const;
        float getMaxRate(bool thin) const;

        HRESULT setDestRect(const Rect& dst);

        HRESULT configMixer(IMFTransform* mixer);
        HRESULT createOptimalVideoType(
            IMFMediaType* proposed_type, IMFMediaType** optimal_type);
        HRESULT calculateOutputRect(IMFMediaType* proposed, RectT<LONG>* output);
        HRESULT setMediaType(IMFMediaType* media_type);
        HRESULT isMediaTypeSupported(IMFMediaType* type);

        HRESULT flush();
        HRESULT renegotiateMediaType();
        HRESULT processInputNotify();
        HRESULT beginStreaming();
        HRESULT endStreaming();
        HRESULT checkEndOfStream();

        void processOutputLoop();
        HRESULT processOutput();
        HRESULT deliverSample(const ComPtr<IMFSample>& sample, bool repaint);
        HRESULT trackSample(IMFSample* sample);
        void destroyResources();

        HRESULT prepareFrameStep(DWORD steps);
        HRESULT startFrameStep();
        HRESULT deliverFrameStepSample(const ComPtr<IMFSample>& sample);
        HRESULT completeFrameStep(IMFSample* sample);
        HRESULT cancelFrameStep();

        HRESULT onSampleFree(IMFAsyncResult* result);

        static RectT<LONG> correctAspectRatio(
            const RectT<LONG>& src, const MFRatio& src_par, const MFRatio& dst_par);
        static HRESULT validateVideoArea(const MFVideoArea& area, UINT32 width, UINT32 height);
        static HRESULT setDesiredSampleTime(IMFSample* sample, LONGLONG sample_time, LONGLONG duration);
        static HRESULT clearDesiredSampleTime(IMFSample* sample);
        static bool isSampleTimePassed(IMFClock* clock, IMFSample* sample);
        static HRESULT setMixerSourceRect(IMFTransform* mixer, const MFVideoNormalizedRect& src);

        volatile ULONG ref_count_;
        win::CritSec obj_lock_;
        RenderState render_state_ = RenderState::Shutdown;
        FrameStep frame_step_;
        MFVideoNormalizedRect src_rect_;
        Size display_size_;

        MFSampleRecycler sample_recycler_;
        MFSampleScheduler scheduler_;

        bool is_sample_notify_ = false;
        bool is_repaint_ = false;
        bool is_prerolled_ = false;
        bool is_end_streaming_ = false;

        float rate_ = 1;
        UINT32 token_counter_ = 0;
        MFD3D9RenderEngine* render_engine_ = nullptr;

        IMediaEventSink* media_event_sink_ = nullptr;
        ComPtr<IMFClock> clock_;
        IMFTransform* mixer_ = nullptr;
        IMFVideoDeviceID* video_device_id_ = nullptr;
        IMFMediaType* media_type_ = nullptr;
    };

}

#endif  // UKIVE_MEDIA_WIN_MF_VIDEO_PRESENTER_H_