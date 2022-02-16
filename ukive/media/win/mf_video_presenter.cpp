// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "mf_video_presenter.h"

#include "utils/log.h"
#include "utils/scope_utils.hpp"

#include <d3d9.h>
#include <evcode.h>
#include <intsafe.h>
#include <mfapi.h>
#include <Mferror.h>

#include "ukive/media/win/mf_async_callback.h"
#include "ukive/media/win/mf_common.h"
#include "ukive/media/win/mf_d3d9_render_engine.h"


namespace {

    MFOffset makeOffset(float val) {
        MFOffset offset;
        offset.value = short(val);
        offset.fract = WORD(65536 * (val - offset.value));
        return offset;
    }

    float fromOffset(const MFOffset& offset) {
        return offset.value + offset.fract / 65536.f;
    }

    bool isMediaTypeEqual(IMFMediaType* t1, IMFMediaType* t2) {
        if (!t1 && !t2) {
            return true;
        }
        if (!t1 || !t2) {
            return false;
        }

        DWORD flags;
        HRESULT hr = t1->IsEqual(t2, &flags);
        return hr == S_OK;
    }

}

namespace ukive {
namespace win {

    MFVideoPresenter::MFVideoPresenter()
        : ref_count_(1),
          obj_lock_(4000)
    {
        src_rect_.left = 0;
        src_rect_.top = 0;
        src_rect_.right = 1;
        src_rect_.bottom = 1;
    }

    MFVideoPresenter::~MFVideoPresenter() {
        destroy();
    }

    HRESULT MFVideoPresenter::initialize() {
        render_engine_ = new MFD3D9RenderEngine();
        if (!render_engine_) {
            return E_OUTOFMEMORY;
        }

        HRESULT hr = render_engine_->initialize();
        if (FAILED(hr)) {
            return hr;
        }

        scheduler_.setCallback(render_engine_);
        return hr;
    }

    void MFVideoPresenter::destroy() {
        clock_.reset();

        if (mixer_) {
            mixer_->Release();
            mixer_ = nullptr;
        }
        if (media_event_sink_) {
            media_event_sink_->Release();
            media_event_sink_ = nullptr;
        }
        if (media_type_) {
            media_type_->Release();
            media_type_ = nullptr;
        }
        if (render_engine_) {
            delete render_engine_;
            render_engine_ = nullptr;
        }
    }

    void MFVideoPresenter::setDisplaySize(const Size& size) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (display_size_ != size) {
            display_size_ = size;

            Rect dst;
            dst.setSize(display_size_);
            setDestRect(dst);
        }
    }

    void MFVideoPresenter::setRenderCallback(MFRenderCallback* cb) {
        if (render_engine_) {
            render_engine_->setRenderCallback(cb);
        }
    }

    STDMETHODIMP MFVideoPresenter::OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        HRESULT hr = S_OK;
        if (isActive()) {
            render_state_ = RenderState::Started;

            if (llClockStartOffset != PRESENTATION_CURRENT_POSITION) {
                flush();
            }
        } else {
            render_state_ = RenderState::Started;
            hr = startFrameStep();
            if (FAILED(hr)) {
                return hr;
            }
        }

        processOutputLoop();
        return hr;
    }

    STDMETHODIMP MFVideoPresenter::OnClockStop(MFTIME hnsSystemTime) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        if (render_state_ == RenderState::Stopped) {
            return S_OK;
        }

        render_state_ = RenderState::Stopped;
        flush();

        if (frame_step_.state != FrameStepState::None) {
            cancelFrameStep();
        }

        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::OnClockPause(MFTIME hnsSystemTime) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        render_state_ = RenderState::Paused;
        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::OnClockRestart(MFTIME hnsSystemTime) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        ubassert(render_state_ == RenderState::Paused);

        render_state_ = RenderState::Started;

        HRESULT hr = startFrameStep();
        if (FAILED(hr)) {
            return hr;
        }

        processOutputLoop();
        return hr;
    }

    STDMETHODIMP MFVideoPresenter::OnClockSetRate(MFTIME hnsSystemTime, float flRate) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        if (rate_ == 0 && flRate != 0) {
            cancelFrameStep();
            frame_step_.samples.clear();
        }

        rate_ = flRate;
        scheduler_.setClockRate(flRate);
        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::ProcessMessage(MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam) {
        utl::win::CritSecGuard guard(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        HRESULT hr;
        switch (eMessage) {
        case MFVP_MESSAGE_FLUSH:
            hr = flush();
            break;

        case MFVP_MESSAGE_INVALIDATEMEDIATYPE:
            hr = renegotiateMediaType();
            break;

        case MFVP_MESSAGE_PROCESSINPUTNOTIFY:
            hr = processInputNotify();
            break;

        case MFVP_MESSAGE_BEGINSTREAMING:
            hr = beginStreaming();
            break;

        case MFVP_MESSAGE_ENDSTREAMING:
            hr = endStreaming();
            break;

        case MFVP_MESSAGE_ENDOFSTREAM:
            is_end_streaming_ = true;
            hr = checkEndOfStream();
            break;

        case MFVP_MESSAGE_STEP:
            hr = prepareFrameStep(LOWORD(ulParam));
            break;

        case MFVP_MESSAGE_CANCELSTEP:
            hr = cancelFrameStep();
            break;

        default:
            hr = E_INVALIDARG;
            break;
        }

        return hr;
    }

    STDMETHODIMP MFVideoPresenter::GetCurrentMediaType(IMFVideoMediaType** ppMediaType) {
        if (!ppMediaType) {
            return E_POINTER;
        }

        *ppMediaType = nullptr;

        utl::win::CritSecGuard csg(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        if (!media_type_) {
            return MF_E_NOT_INITIALIZED;
        }

        return media_type_->QueryInterface(IID_PPV_ARGS(ppMediaType));
    }

    STDMETHODIMP MFVideoPresenter::GetService(
        const GUID& guidService, const IID& riid, LPVOID* ppvObject)
    {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (guidService != MR_VIDEO_RENDER_SERVICE) {
            return MF_E_UNSUPPORTED_SERVICE;
        }

        HRESULT hr = render_engine_->getService(guidService, riid, ppvObject);
        if (FAILED(hr)) {
            hr = QueryInterface(riid, ppvObject);
        }

        return hr;
    }

    STDMETHODIMP MFVideoPresenter::InitServicePointers(IMFTopologyServiceLookup* pLookup) {
        if (!pLookup) {
            return E_POINTER;
        }

        utl::win::CritSecGuard csg(obj_lock_);

        if (isActive()) {
            return MF_E_INVALIDREQUEST;
        }

        clock_.reset();

        if (mixer_) {
            mixer_->Release();
        }
        if (media_event_sink_) {
            media_event_sink_->Release();
        }

        DWORD obj_count = 1;
        pLookup->LookupService(
            MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&clock_), &obj_count);

        obj_count = 1;
        HRESULT hr = pLookup->LookupService(
            MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_MIXER_SERVICE, IID_PPV_ARGS(&mixer_), &obj_count);
        if (FAILED(hr)) {
            return hr;
        }

        hr = configMixer(mixer_);
        if (FAILED(hr)) {
            return hr;
        }

        obj_count = 1;
        hr = pLookup->LookupService(
            MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&media_event_sink_), &obj_count);
        if (FAILED(hr)) {
            return hr;
        }

        render_state_ = RenderState::Stopped;
        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::ReleaseServicePointers() {
        {
            utl::win::CritSecGuard csg(obj_lock_);
            render_state_ = RenderState::Shutdown;
        }

        flush();
        setMediaType(nullptr);

        clock_.reset();

        if (mixer_) {
            mixer_->Release();
            mixer_ = nullptr;
        }
        if (media_event_sink_) {
            media_event_sink_->Release();
            media_event_sink_ = nullptr;
        }

        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::GetDeviceID(IID* pDeviceID) {
        if (!pDeviceID) {
            return E_POINTER;
        }
        *pDeviceID = __uuidof(IDirect3DDevice9);
        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::GetNativeVideoSize(SIZE* pszVideo, SIZE* pszARVideo) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::GetIdealVideoSize(SIZE* pszMin, SIZE* pszMax) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::SetVideoPosition(const MFVideoNormalizedRect* pnrcSource, const LPRECT prcDest) {
        utl::win::CritSecGuard csg(obj_lock_);

        if (!pnrcSource && !prcDest) {
            return E_POINTER;
        }

        if (pnrcSource) {
            if (pnrcSource->left > pnrcSource->right ||
                pnrcSource->top > pnrcSource->bottom)
            {
                return E_INVALIDARG;
            }
            if (pnrcSource->left < 0 ||
                pnrcSource->right > 1 ||
                pnrcSource->top < 0 ||
                pnrcSource->bottom > 1)
            {
                return E_INVALIDARG;
            }
        }

        if (prcDest) {
            if (prcDest->left > prcDest->right ||
                prcDest->top > prcDest->bottom)
            {
                return E_INVALIDARG;
            }
        }

        HRESULT hr = S_OK;
        if (pnrcSource) {
            src_rect_ = *pnrcSource;
            if (mixer_) {
                hr = setMixerSourceRect(mixer_, src_rect_);
                if (FAILED(hr)) {
                    return hr;
                }
            }
        }

        if (prcDest) {
            Rect dst;
            if (display_size_.empty()) {
                dst.set(
                    prcDest->left, prcDest->top,
                    prcDest->right - prcDest->left, prcDest->bottom - prcDest->top);
            } else {
                dst.setSize(display_size_);
            }
            hr = setDestRect(dst);
        }

        return hr;
    }

    STDMETHODIMP MFVideoPresenter::GetVideoPosition(MFVideoNormalizedRect* pnrcSource, LPRECT prcDest) {
        utl::win::CritSecGuard csg(obj_lock_);

        if (!pnrcSource || !prcDest) {
            return E_POINTER;
        }

        *pnrcSource = src_rect_;

        auto dst = render_engine_->getDestinationRect();
        prcDest->left = dst.left;
        prcDest->top = dst.top;
        prcDest->right = dst.right;
        prcDest->bottom = dst.bottom;

        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::SetAspectRatioMode(DWORD dwAspectRatioMode) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::GetAspectRatioMode(DWORD* pdwAspectRatioMode) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::SetVideoWindow(HWND hwndVideo) {
        utl::win::CritSecGuard csg(obj_lock_);

        if (!::IsWindow(hwndVideo)) {
            return E_INVALIDARG;
        }

        HRESULT hr = S_OK;
        auto old_hwnd = render_engine_->getVideoWindow();
        if (old_hwnd != hwndVideo) {
            hr = render_engine_->setVideoWindow(hwndVideo);
            if (media_event_sink_) {
                media_event_sink_->Notify(EC_DISPLAY_CHANGED, 0, 0);
            }
        }
        return hr;
    }

    STDMETHODIMP MFVideoPresenter::GetVideoWindow(HWND* phwndVideo) {
        utl::win::CritSecGuard csg(obj_lock_);

        if (!phwndVideo) {
            return E_POINTER;
        }

        *phwndVideo = render_engine_->getVideoWindow();
        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::RepaintVideo() {
        utl::win::CritSecGuard csg(obj_lock_);

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        if (is_prerolled_) {
            is_repaint_ = true;
            processOutput();
        }

        return S_OK;
    }

    STDMETHODIMP MFVideoPresenter::GetCurrentImage(
        BITMAPINFOHEADER* pBih, BYTE** pDib, DWORD* pcbDib, LONGLONG* pTimeStamp)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::SetBorderColor(COLORREF Clr) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::GetBorderColor(COLORREF* pClr) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::SetRenderingPrefs(DWORD dwRenderFlags) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::GetRenderingPrefs(DWORD* pdwRenderFlags) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::SetFullscreen(BOOL fFullscreen) {
        return E_NOTIMPL;
    }

    STDMETHODIMP MFVideoPresenter::GetFullscreen(BOOL* pfFullscreen) {
        return E_NOTIMPL;
    }

    STDMETHODIMP_(ULONG) MFVideoPresenter::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) MFVideoPresenter::Release() {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    STDMETHODIMP MFVideoPresenter::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObject = static_cast<IUnknown*>(static_cast<IMFVideoPresenter*>(this));
        } else if (IsEqualIID(riid, __uuidof(IMFClockStateSink))) {
            *ppvObject = static_cast<IMFClockStateSink*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFVideoPresenter))) {
            *ppvObject = static_cast<IMFVideoPresenter*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFGetService))) {
            *ppvObject = static_cast<IMFGetService*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFTopologyServiceLookupClient))) {
            *ppvObject = static_cast<IMFTopologyServiceLookupClient*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFVideoDeviceID))) {
            *ppvObject = static_cast<IMFVideoDeviceID*>(this);
        } else if (IsEqualIID(riid, __uuidof(IMFVideoDisplayControl))) {
            *ppvObject = static_cast<IMFVideoDisplayControl*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    bool MFVideoPresenter::isActive() const {
        return render_state_ == RenderState::Started ||
            render_state_ == RenderState::Paused;
    }

    bool MFVideoPresenter::isScrubbing() const {
        return rate_ == 0;
    }

    float MFVideoPresenter::getMaxRate(bool thin) const {
        if (!thin && media_type_) {
            UINT32 numerator, denominator;
            HRESULT hr = ::MFGetAttributeRatio(media_type_, MF_MT_FRAME_RATE, &numerator, &denominator);
            if (FAILED(hr)) {
                numerator = 0;
                denominator = 0;
            }
            UINT monitor_rate = render_engine_->getRefreshRate();
            if (denominator && numerator && monitor_rate) {
                return float(MulDiv(monitor_rate, denominator, numerator));
            }
        }
        return FLT_MAX;
    }

    HRESULT MFVideoPresenter::setDestRect(const Rect& dst) {
        HRESULT hr = S_OK;
        auto old_rect = render_engine_->getDestinationRect();
        if (old_rect != dst) {
            render_engine_->setDestinationRect(dst);
            if (mixer_) {
                hr = renegotiateMediaType();
                if (hr == MF_E_TRANSFORM_TYPE_NOT_SET) {
                    hr = S_OK;
                } else {
                    if (FAILED(hr)) {
                        return hr;
                    }

                    is_repaint_ = true;
                    processOutput();
                }
            }
        }
        return hr;
    }

    HRESULT MFVideoPresenter::configMixer(IMFTransform* mixer) {
        HRESULT hr;
        utl::win::ComPtr<IMFVideoDeviceID> device_id;
        hr = mixer->QueryInterface(&device_id);
        if (FAILED(hr)) {
            return hr;
        }

        IID devid;
        hr = device_id->GetDeviceID(&devid);
        if (FAILED(hr)) {
            return hr;
        }

        if (!IsEqualGUID(devid, __uuidof(IDirect3DDevice9))) {
            return MF_E_INVALIDREQUEST;
        }

        setMixerSourceRect(mixer, src_rect_);
        return hr;
    }

    HRESULT MFVideoPresenter::createOptimalVideoType(
        IMFMediaType* proposed_type, IMFMediaType** optimal_type)
    {
        MFVideoArea display_area;
        ZeroMemory(&display_area, sizeof(MFVideoArea));

        HRESULT hr;
        utl::win::ComPtr<IMFMediaType> optimal;
        hr = ::MFCreateMediaType(&optimal);
        if (FAILED(hr)) {
            return hr;
        }

        hr = proposed_type->CopyAllItems(optimal.get());
        if (FAILED(hr)) {
            return hr;
        }

        // 这里假设：显示器像素为正方形
        hr = MFSetAttributeRatio(optimal.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
        if (FAILED(hr)) {
            return hr;
        }

        Rect out_rect = render_engine_->getDestinationRect();
        if (out_rect.empty()) {
            RectT<LONG> out;
            hr = calculateOutputRect(proposed_type, &out);
            if (FAILED(hr)) {
                return hr;
            }
            out_rect = Rect(out);
        }

        // BT.709
        hr = optimal->SetUINT32(MF_MT_YUV_MATRIX, MFVideoTransferMatrix_BT709);
        if (FAILED(hr)) {
            return hr;
        }
        hr = optimal->SetUINT32(MF_MT_TRANSFER_FUNCTION, MFVideoTransFunc_709);
        if (FAILED(hr)) {
            return hr;
        }
        hr = optimal->SetUINT32(MF_MT_VIDEO_PRIMARIES, MFVideoPrimaries_BT709);
        if (FAILED(hr)) {
            return hr;
        }
        hr = optimal->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_16_235);
        if (FAILED(hr)) {
            return hr;
        }
        hr = optimal->SetUINT32(MF_MT_VIDEO_LIGHTING, MFVideoLighting_dim);
        if (FAILED(hr)) {
            return hr;
        }

        // 设置目标大小
        hr = MFSetAttributeSize(optimal.get(), MF_MT_FRAME_SIZE, out_rect.right, out_rect.bottom);
        if (FAILED(hr)) {
            return hr;
        }

        // 设置显示位置
        display_area.OffsetX = makeOffset(0);
        display_area.OffsetY = makeOffset(0);
        display_area.Area.cx = out_rect.right;
        display_area.Area.cy = out_rect.bottom;

        hr = optimal->SetUINT32(MF_MT_PAN_SCAN_ENABLED, FALSE);
        if (FAILED(hr)) {
            return hr;
        }

        hr = optimal->SetBlob(
            MF_MT_GEOMETRIC_APERTURE,
            reinterpret_cast<UINT8*>(&display_area), sizeof(MFVideoArea));
        if (FAILED(hr)) {
            return hr;
        }

        // 设置 PAN/SCAN aperture
        hr = optimal->SetBlob(
            MF_MT_PAN_SCAN_APERTURE,
            reinterpret_cast<UINT8*>(&display_area), sizeof(MFVideoArea));
        if (FAILED(hr)) {
            return hr;
        }

        hr = optimal->SetBlob(
            MF_MT_MINIMUM_DISPLAY_APERTURE,
            reinterpret_cast<UINT8*>(&display_area), sizeof(MFVideoArea));
        if (FAILED(hr)) {
            return hr;
        }

        *optimal_type = optimal.detach();
        return S_OK;
    }

    HRESULT MFVideoPresenter::calculateOutputRect(IMFMediaType* proposed, RectT<LONG>* output) {
        UINT32 src_width, src_height;
        HRESULT hr = ::MFGetAttributeSize(proposed, MF_MT_FRAME_SIZE, &src_width, &src_height);
        if (FAILED(hr)) {
            return hr;
        }

        MFVideoArea display_area;
        {
            auto pan_scan = ::MFGetAttributeUINT32(proposed, MF_MT_PAN_SCAN_ENABLED, FALSE);
            if (pan_scan) {
                hr = proposed->GetBlob(
                    MF_MT_PAN_SCAN_APERTURE,
                    reinterpret_cast<UINT8*>(&display_area), sizeof(MFVideoArea), nullptr);
            }

            if (!pan_scan || hr == MF_E_ATTRIBUTENOTFOUND) {
                hr = proposed->GetBlob(
                    MF_MT_MINIMUM_DISPLAY_APERTURE,
                    reinterpret_cast<UINT8*>(&display_area), sizeof(MFVideoArea), nullptr);
                if (hr == MF_E_ATTRIBUTENOTFOUND) {
                    hr = proposed->GetBlob(MF_MT_GEOMETRIC_APERTURE,
                        reinterpret_cast<UINT8*>(&display_area), sizeof(MFVideoArea), nullptr);
                }
                if (hr == MF_E_ATTRIBUTENOTFOUND) {
                    UINT32 width, height;
                    hr = ::MFGetAttributeSize(proposed, MF_MT_FRAME_SIZE, &width, &height);
                    if (SUCCEEDED(hr)) {
                        display_area.Area.cx = width;
                        display_area.Area.cy = height;
                        display_area.OffsetX = makeOffset(0);
                        display_area.OffsetY = makeOffset(0);
                    }
                }
            }

            if (FAILED(hr)) {
                return hr;
            }
        }

        RectT<LONG> out;
        LONG offset_x = LONG(fromOffset(display_area.OffsetX));
        LONG offset_y = LONG(fromOffset(display_area.OffsetY));

        if (display_area.Area.cx !=0 &&
            display_area.Area.cy != 0 &&
            offset_x + display_area.Area.cx <= LONG(src_width) &&
            offset_y + display_area.Area.cy <= LONG(src_height))
        {
            out.set(offset_x, offset_y, display_area.Area.cx, display_area.Area.cy);
        } else {
            out.set(0, 0, src_width, src_height);
        }

        MFRatio input_par;
        {
            UINT32 numerator, denominator;
            hr = ::MFGetAttributeRatio(
                proposed,
                MF_MT_PIXEL_ASPECT_RATIO,
                &numerator,
                &denominator);
            if (SUCCEEDED(hr)) {
                input_par.Numerator = numerator;
                input_par.Denominator = denominator;
            } else {
                input_par.Numerator = 1;
                input_par.Denominator = 1;
            }
        }

        MFRatio output_par;
        output_par.Numerator = output_par.Denominator = 1;

        *output = correctAspectRatio(out, input_par, output_par);
        return hr;
    }

    HRESULT MFVideoPresenter::setMediaType(IMFMediaType* media_type) {
        if (!media_type) {
            if (media_type_) {
                media_type_->Release();
                media_type_ = nullptr;
            }
            destroyResources();
            return S_OK;
        }

        HRESULT hr;
        ESC_FROM_SCOPE {
            if (FAILED(hr)) {
                destroyResources();
            }
        };

        if (render_state_ == RenderState::Shutdown) {
            return MF_E_SHUTDOWN;
        }

        if (isMediaTypeEqual(media_type_, media_type)) {
            return S_OK;
        }

        if (media_type_) {
            media_type_->Release();
            media_type_ = nullptr;
        }
        destroyResources();

        SampleList sample_list;
        hr = render_engine_->createVideoSamples(media_type, sample_list);
        if (FAILED(hr)) {
            return hr;
        }

        for (const auto& sample : sample_list) {
            hr = sample->SetUINT32(MFSampleCounter, token_counter_);
            if (FAILED(hr)) {
                return hr;
            }
        }

        sample_recycler_.recycleSamples(sample_list);

        MFRatio fps;
        UINT32 numerator, denominator;
        hr = ::MFGetAttributeRatio(
            media_type,
            MF_MT_FRAME_RATE,
            &numerator,
            &denominator);
        if (SUCCEEDED(hr) && numerator != 0 && denominator != 0) {
            fps.Numerator = numerator;
            fps.Denominator = denominator;
            scheduler_.setFrameRate(fps);
        } else {
            // 默认值
            scheduler_.setFrameRate({ 30, 1 });
        }

        media_type_ = media_type;
        media_type_->AddRef();
        return hr;
    }

    HRESULT MFVideoPresenter::isMediaTypeSupported(IMFMediaType* type) {
        BOOL is_compressed;
        HRESULT hr = type->IsCompressedFormat(&is_compressed);
        if (FAILED(hr)) {
            return hr;
        }

        if (is_compressed) {
            return MF_E_INVALIDMEDIATYPE;
        }

        GUID subtype_guid;
        hr = type->GetGUID(MF_MT_SUBTYPE, &subtype_guid);
        if (FAILED(hr)) {
            return hr;
        }

        D3DFORMAT d3d_format = D3DFORMAT(subtype_guid.Data1);
        hr = render_engine_->checkFormat(d3d_format);
        if (FAILED(hr)) {
            return hr;
        }

        MFVideoInterlaceMode interlace_mode;
        hr = type->GetUINT32(
            MF_MT_INTERLACE_MODE, reinterpret_cast<UINT32*>(&interlace_mode));
        if (FAILED(hr)) {
            return hr;
        }
        if (interlace_mode != MFVideoInterlace_Progressive) {
            return MF_E_INVALIDMEDIATYPE;
        }

        UINT32 width, height;
        hr = ::MFGetAttributeSize(type, MF_MT_FRAME_SIZE, &width, &height);
        if (FAILED(hr)) {
            return hr;
        }

        MFVideoArea video_crop_area;
        if (SUCCEEDED(type->GetBlob(
            MF_MT_PAN_SCAN_APERTURE,
            reinterpret_cast<UINT8*>(&video_crop_area), sizeof(MFVideoArea), nullptr)))
        {
            validateVideoArea(video_crop_area, width, height);
        }

        if (SUCCEEDED(type->GetBlob(
            MF_MT_GEOMETRIC_APERTURE,
            reinterpret_cast<UINT8*>(&video_crop_area), sizeof(MFVideoArea), nullptr)))
        {
            validateVideoArea(video_crop_area, width, height);
        }

        if (SUCCEEDED(type->GetBlob(
            MF_MT_MINIMUM_DISPLAY_APERTURE,
            reinterpret_cast<UINT8*>(&video_crop_area), sizeof(MFVideoArea), nullptr)))
        {
            validateVideoArea(video_crop_area, width, height);
        }

        return hr;
    }

    HRESULT MFVideoPresenter::flush() {
        is_prerolled_ = false;

        scheduler_.flush();
        frame_step_.samples.clear();

        if (render_state_ == RenderState::Stopped) {
            render_engine_->presentSample(nullptr, 0);
        }

        return S_OK;
    }

    HRESULT MFVideoPresenter::renegotiateMediaType() {
        if (!mixer_) {
            return MF_E_INVALIDREQUEST;
        }

        IMFMediaType* mixer_type = nullptr;
        IMFMediaType* optimal_type = nullptr;
        IMFVideoMediaType* video_type = nullptr;

        HRESULT hr = S_OK;
        DWORD type_index = 0;
        bool found_media_type = false;
        while (!found_media_type && hr != MF_E_NO_MORE_TYPES) {
            if (mixer_type) {
                mixer_type->Release();
                mixer_type = nullptr;
            }
            if (optimal_type) {
                optimal_type->Release();
                optimal_type = nullptr;
            }

            hr = mixer_->GetOutputAvailableType(0, type_index++, &mixer_type);
            if (FAILED(hr)) {
                break;
            }

            if (SUCCEEDED(hr)) {
                hr = isMediaTypeSupported(mixer_type);
            }
            if (SUCCEEDED(hr)) {
                hr = createOptimalVideoType(mixer_type, &optimal_type);
            }
            if (SUCCEEDED(hr)) {
                hr = mixer_->SetOutputType(0, optimal_type, MFT_SET_TYPE_TEST_ONLY);
            }
            if (SUCCEEDED(hr)) {
                hr = setMediaType(optimal_type);
            }
            if (SUCCEEDED(hr)) {
                hr = mixer_->SetOutputType(0, optimal_type, 0);
                if (FAILED(hr)) {
                    ubassert(false);
                    setMediaType(nullptr);
                }
            }
            if (SUCCEEDED(hr)) {
                found_media_type = true;
            }
        }

        if (mixer_type) {
            mixer_type->Release();
        }
        if (optimal_type) {
            optimal_type->Release();
        }
        if (video_type) {
            video_type->Release();
        }

        return hr;
    }

    HRESULT MFVideoPresenter::processInputNotify() {
        is_sample_notify_ = true;

        if (!media_type_) {
            return MF_E_TRANSFORM_TYPE_NOT_SET;
        }

        processOutputLoop();
        return S_OK;
    }

    HRESULT MFVideoPresenter::beginStreaming() {
        return scheduler_.start(clock_);
    }

    HRESULT MFVideoPresenter::endStreaming() {
        scheduler_.stop();
        return S_OK;
    }

    HRESULT MFVideoPresenter::checkEndOfStream() {
        if (!is_end_streaming_) {
            return S_OK;
        }
        if (is_sample_notify_) {
            return S_OK;
        }
        if (sample_recycler_.isPending()) {
            return S_OK;
        }

        if (media_event_sink_) {
            media_event_sink_->Notify(EC_COMPLETE, S_OK, 0);
        }
        is_end_streaming_ = false;
        return S_OK;
    }

    void MFVideoPresenter::processOutputLoop() {
        HRESULT hr = S_OK;
        while (hr == S_OK) {
            if (!is_sample_notify_) {
                hr = MF_E_TRANSFORM_NEED_MORE_INPUT;
                break;
            }

            hr = processOutput();
        }

        if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
            checkEndOfStream();
        }
    }

    HRESULT MFVideoPresenter::processOutput() {
        ubassert(is_sample_notify_ || is_repaint_);

        if (render_state_ != RenderState::Started &&
            !is_repaint_ && is_prerolled_)
        {
            return S_FALSE;
        }

        if (!mixer_) {
            return MF_E_INVALIDREQUEST;
        }

        bool is_repaint = is_repaint_;

        auto sample = sample_recycler_.reuseSample();
        if (!sample) {
            return S_FALSE;
        }

        ubassert(::MFGetAttributeUINT32(sample.get(), MFSampleCounter, UINT32(-1)) == token_counter_);

        LONGLONG mixer_start_time = 0;
        LONGLONG mixer_end_time = 0;
        MFTIME system_time = 0;

        if (is_repaint_) {
            setDesiredSampleTime(
                sample.get(), scheduler_.getLastSampleTime(), scheduler_.getFrameDuration());
            is_repaint_ = false;
        } else {
            clearDesiredSampleTime(sample.get());
            if (clock_) {
                clock_->GetCorrelatedTime(0, &mixer_start_time, &system_time);
            }
        }

        DWORD status = 0;
        MFT_OUTPUT_DATA_BUFFER data_buf;
        ZeroMemory(&data_buf, sizeof(data_buf));

        data_buf.dwStreamID = 0;
        data_buf.pSample = sample.get();
        data_buf.dwStatus = 0;

        HRESULT hr = mixer_->ProcessOutput(0, 1, &data_buf, &status);
        auto db_events = utl::win::ComPtr(data_buf.pEvents);
        if (FAILED(hr)) {
            sample_recycler_.recycleSample(sample);
            if (hr == MF_E_TRANSFORM_TYPE_NOT_SET) {
                hr = renegotiateMediaType();
            } else if (hr == MF_E_TRANSFORM_STREAM_CHANGE) {
                setMediaType(nullptr);
            } else if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
                is_sample_notify_ = false;
            }
        } else {
            if (clock_ && is_repaint) {
                clock_->GetCorrelatedTime(0, &mixer_end_time, &system_time);

                LONGLONG latency_time = mixer_end_time - mixer_start_time;
                if (media_event_sink_) {
                    media_event_sink_->Notify(
                        EC_PROCESSING_LATENCY, reinterpret_cast<LONG_PTR>(&latency_time), 0);
                }
            }

            hr = trackSample(sample.get());
            if (FAILED(hr)) {
                return hr;
            }

            if (frame_step_.state == FrameStepState::None || is_repaint) {
                hr = deliverSample(sample, is_repaint);
                if (FAILED(hr)) {
                    return hr;
                }
            } else {
                hr = deliverFrameStepSample(sample);
                if (FAILED(hr)) {
                    return hr;
                }
            }

            is_prerolled_ = true;
        }

        return hr;
    }

    HRESULT MFVideoPresenter::deliverSample(const utl::win::ComPtr<IMFSample>& sample, bool repaint) {
        ubassert(sample);
        bool present_now = render_state_ != RenderState::Started || isScrubbing() || repaint;

        MFD3D9RenderEngine::DevState state;
        HRESULT hr = render_engine_->checkDeviceState(&state);
        if (SUCCEEDED(hr)) {
            hr = scheduler_.scheduleSample(sample, present_now);
        }

        if (FAILED(hr)) {
            if (media_event_sink_) {
                media_event_sink_->Notify(EC_ERRORABORT, hr, 0);
            }
        } else if (state == MFD3D9RenderEngine::DevState::Reset) {
            if (media_event_sink_) {
                media_event_sink_->Notify(EC_DISPLAY_CHANGED, S_OK, 0);
            }
        }

        return hr;
    }

    HRESULT MFVideoPresenter::trackSample(IMFSample* sample) {
        utl::win::ComPtr<IMFTrackedSample> tracked;
        HRESULT hr = sample->QueryInterface(&tracked);
        if (FAILED(hr)) {
            return hr;
        }

        auto callback = new MFAsyncCallback(
            std::bind(&MFVideoPresenter::onSampleFree, this, std::placeholders::_1));

        hr = tracked->SetAllocator(callback, nullptr);
        callback->Release();

        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

    void MFVideoPresenter::destroyResources() {
        ++token_counter_;
        flush();

        sample_recycler_.clear();
        render_engine_->destroyResources();
    }

    HRESULT MFVideoPresenter::prepareFrameStep(DWORD steps) {
        frame_step_.steps += steps;
        frame_step_.state = FrameStepState::Waiting;

        if (render_state_ == RenderState::Started) {
            return startFrameStep();
        }
        return S_OK;
    }

    HRESULT MFVideoPresenter::startFrameStep() {
        ubassert(render_state_ == RenderState::Started);

        HRESULT hr = S_OK;
        if (frame_step_.state == FrameStepState::Waiting) {
            frame_step_.state = FrameStepState::Pending;
            while (!frame_step_.samples.empty() &&
                frame_step_.state == FrameStepState::Pending)
            {
                auto sample = frame_step_.samples.front();
                frame_step_.samples.pop_front();
                hr = deliverFrameStepSample(sample);
                if (FAILED(hr)) {
                    return hr;
                }
            }
        } else if (frame_step_.state == FrameStepState::None) {
            while (!frame_step_.samples.empty()) {
                auto sample = frame_step_.samples.front();
                frame_step_.samples.pop_front();
                hr = deliverSample(sample, false);
                if (FAILED(hr)) {
                    return hr;
                }
            }
        }

        return hr;
    }

    HRESULT MFVideoPresenter::deliverFrameStepSample(const utl::win::ComPtr<IMFSample>& sample) {
        HRESULT hr = S_OK;
        if (isScrubbing() && clock_ && isSampleTimePassed(clock_.get(), sample.get())) {

        } else if (frame_step_.state >= FrameStepState::Scheduled) {
            frame_step_.samples.push_back(sample);
        } else {
            if (frame_step_.steps > 0) {
                --frame_step_.steps;
            }
            if (frame_step_.steps > 0) {

            } else if (frame_step_.state == FrameStepState::Waiting) {
                frame_step_.samples.push_back(sample);
            } else {
                hr = deliverSample(sample, false);
                if (FAILED(hr)) {
                    return hr;
                }

                utl::win::ComPtr<IUnknown> unknown;
                hr = sample->QueryInterface(&unknown);
                if (FAILED(hr)) {
                    return hr;
                }

                frame_step_.sample_no_ref = DWORD_PTR(unknown.get());
                frame_step_.state = FrameStepState::Scheduled;
            }
        }

        return hr;
    }

    HRESULT MFVideoPresenter::completeFrameStep(IMFSample* sample) {
        frame_step_.state = FrameStepState::Complete;
        frame_step_.sample_no_ref = 0;

        if (media_event_sink_) {
            media_event_sink_->Notify(EC_STEP_COMPLETE, FALSE, 0);
        }

        HRESULT hr = S_OK;
        if (isScrubbing()) {
            LONGLONG sample_time;
            LONGLONG system_time;
            hr = sample->GetSampleTime(&sample_time);
            if (FAILED(hr)) {
                if (clock_) {
                    hr = clock_->GetCorrelatedTime(0, &sample_time, &system_time);
                    if (FAILED(hr)) {
                        sample_time = 0;
                        system_time = 0;
                    }
                }
                hr = S_OK;
            }

            if (media_event_sink_) {
                media_event_sink_->Notify(
                    EC_SCRUB_TIME, LODWORD(sample_time), HIDWORD(sample_time));
            }
        }

        return hr;
    }

    HRESULT MFVideoPresenter::cancelFrameStep() {
        auto prev_state = frame_step_.state;

        frame_step_.state = FrameStepState::None;
        frame_step_.steps = 0;
        frame_step_.sample_no_ref = 0;

        if (prev_state > FrameStepState::None &&
            prev_state < FrameStepState::Complete)
        {
            if (media_event_sink_) {
                media_event_sink_->Notify(EC_STEP_COMPLETE, TRUE, 0);
            }
        }
        return S_OK;
    }

    HRESULT MFVideoPresenter::onSampleFree(IMFAsyncResult* result) {
        HRESULT hr;
        utl::win::ComPtr<IUnknown> object;
        utl::win::ComPtr<IMFSample> sample;
        utl::win::ComPtr<IUnknown> unknown;

        ESC_FROM_SCOPE {
            if (FAILED(hr)) {
                if (media_event_sink_) {
                    media_event_sink_->Notify(EC_ERRORABORT, hr, 0);
                }
            }
        };

        hr = result->GetObjectW(&object);
        if (FAILED(hr)) {
            return hr;
        }

        hr = object->QueryInterface(&sample);
        if (FAILED(hr)) {
            return hr;
        }

        if (frame_step_.state == FrameStepState::Scheduled) {
            hr = sample->QueryInterface(_uuidof(IMFSample), IID_PPV_ARGS_Helper(&unknown));
            if (FAILED(hr)) {
                return hr;
            }

            if (frame_step_.sample_no_ref == DWORD_PTR(unknown.get())) {
                hr = completeFrameStep(sample.get());
                if (FAILED(hr)) {
                    return hr;
                }
            }
        }

        obj_lock_.lock();

        if (::MFGetAttributeUINT32(sample.get(), MFSampleCounter, UINT32(-1)) == token_counter_) {
            sample_recycler_.recycleSample(sample);
            processOutputLoop();
        }

        obj_lock_.unlock();
        return hr;
    }

    // static
    RectT<LONG> MFVideoPresenter::correctAspectRatio(
        const RectT<LONG>& src, const MFRatio& src_par, const MFRatio& dst_par)
    {
        RectT<LONG> rect(0, 0, src.width(), src.height());

        if (src_par.Numerator != dst_par.Numerator ||
            src_par.Denominator != dst_par.Denominator)
        {
            if (src_par.Numerator > src_par.Denominator) {
                rect.right = MulDiv(rect.right, src_par.Numerator, src_par.Denominator);
            } else if (src_par.Numerator < src_par.Denominator) {
                rect.bottom = MulDiv(rect.bottom, src_par.Denominator, src_par.Numerator);
            }

            if (dst_par.Numerator > dst_par.Denominator) {
                rect.bottom = MulDiv(rect.bottom, dst_par.Numerator, dst_par.Denominator);
            } else if (dst_par.Numerator < dst_par.Denominator) {
                rect.right = MulDiv(rect.right, dst_par.Denominator, dst_par.Numerator);
            }
        }

        return rect;
    }

    // static
    HRESULT MFVideoPresenter::validateVideoArea(
        const MFVideoArea& area, UINT32 width, UINT32 height)
    {
        float offset_x = fromOffset(area.OffsetX);
        float offset_y = fromOffset(area.OffsetY);

        if (LONG(offset_x) + area.Area.cx > LONG(width) ||
            LONG(offset_y) + area.Area.cy > LONG(height))
        {
            return MF_E_INVALIDMEDIATYPE;
        }
        return S_OK;
    }

    // static
    HRESULT MFVideoPresenter::setDesiredSampleTime(
        IMFSample* sample, LONGLONG sample_time, LONGLONG duration)
    {
        if (!sample) {
            return E_POINTER;
        }

        utl::win::ComPtr<IMFDesiredSample> desired;
        HRESULT hr = sample->QueryInterface(&desired);
        if (SUCCEEDED(hr)) {
            desired->SetDesiredSampleTimeAndDuration(sample_time, duration);
        }

        return hr;
    }

    // static
    HRESULT MFVideoPresenter::clearDesiredSampleTime(IMFSample* sample) {
        if (!sample) {
            return E_POINTER;
        }

        UINT32 counter = ::MFGetAttributeUINT32(sample, MFSampleCounter, UINT32(-1));

        utl::win::ComPtr<IUnknown> unknown;
        sample->GetUnknown(MFSampleSwapChain, IID_PPV_ARGS(&unknown));

        utl::win::ComPtr<IMFDesiredSample> desired;
        HRESULT hr = sample->QueryInterface(&desired);
        if (SUCCEEDED(hr)) {
            desired->Clear();
            hr = sample->SetUINT32(MFSampleCounter, counter);
            if (FAILED(hr)) {
                return hr;
            }

            if (unknown) {
                hr = sample->SetUnknown(MFSampleSwapChain, unknown.get());
                if (FAILED(hr)) {
                    return hr;
                }
            }
        }
        return hr;
    }

    // static
    bool MFVideoPresenter::isSampleTimePassed(IMFClock* clock, IMFSample* sample) {
        ubassert(clock && sample);
        if (!sample || !clock) {
            return false;
        }

        MFTIME time_now;
        MFTIME system_time;
        MFTIME sample_start;
        MFTIME sample_duration;
        HRESULT hr = clock->GetCorrelatedTime(0, &time_now, &system_time);
        if (SUCCEEDED(hr)) {
            hr = sample->GetSampleTime(&sample_start);
        }
        if (SUCCEEDED(hr)) {
            hr = sample->GetSampleDuration(&sample_duration);
        }
        if (SUCCEEDED(hr)) {
            if (sample_start + sample_duration < time_now) {
                return true;
            }
        }

        return false;
    }

    // static
    HRESULT MFVideoPresenter::setMixerSourceRect(
        IMFTransform* mixer, const MFVideoNormalizedRect& src)
    {
        if (!mixer) {
            return E_POINTER;
        }

        utl::win::ComPtr<IMFAttributes> attrs;
        HRESULT hr = mixer->GetAttributes(&attrs);
        if (FAILED(hr)) {
            return hr;
        }

        hr = attrs->SetBlob(VIDEO_ZOOM_RECT, reinterpret_cast<const UINT8*>(&src), sizeof(src));
        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

}
}