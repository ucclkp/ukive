// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "media_player_win.h"

#include <evr.h>
#include <mfapi.h>

#include "utils/log.h"
#include "utils/message/message.h"
#include "utils/scope_utils.h"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/win/gpu/gpu_context_d3d.h"
#include "ukive/graphics/win/gpu/gpu_device_d3d.h"
#include "ukive/window/win/window_impl_win.h"
#include "ukive/media/win/mf_async_callback.h"
#include "ukive/media/win/mf_video_presenter_activate.h"
#include "ukive/system/win/com_ptr.hpp"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/win/cyro_render_target_d2d.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/window/window.h"

#define MSG_CREATE_SURFACE   1
#define MSG_DESTROY_SURFACE  2
#define MSG_RENDER_SURFACE   3
#define MSG_OPEN_SUCCEEDED   4
#define MSG_OPEN_FAILED      5
#define MSG_MEDIA_STARTED    6
#define MSG_MEDIA_PAUSED     7
#define MSG_MEDIA_STOPPED    8
#define MSG_MEDIA_ENDED      9
#define MSG_MEDIA_CLOSED     10


namespace ukive {

    // static
    unsigned int MediaPlayerWin::startup_counter_ = 0;

    // static
    bool MediaPlayerWin::startup() {
        HRESULT hr;
        if (startup_counter_ == 0) {
            hr = ::MFStartup(MF_VERSION, MFSTARTUP_FULL);
            if (FAILED(hr)) {
                LOG(Log::ERR) << "Failed to initialize Media Foundation: " << hr;
                return false;
            }
        }
        ++startup_counter_;
        return true;
    }

    // static
    void MediaPlayerWin::shutdown() {
        if (startup_counter_ > 0) {
            --startup_counter_;
            if (startup_counter_ == 0) {
                ::MFShutdown();
            }
        }
    }

    MediaPlayerWin::MediaPlayerWin() {
        cycler_.setListener(this);

        vp_activate_ = new MFVideoPresenterActivate();
        vp_activate_->setRenderCallback(this);
    }

    MediaPlayerWin::~MediaPlayerWin() {
        destroy();
    }

    void MediaPlayerWin::setCallback(MediaPlayerCallback* cb) {
        callback_ = cb;
    }

    void MediaPlayerWin::setDisplaySize(const Size& size) {
        vp_activate_->setDisplaySize(size);
    }

    bool MediaPlayerWin::openUrl(const std::u16string& url, Window* w) {
        if (state_ != State::Idle) {
            return false;
        }

        window_ = w;
        hwnd_ = static_cast<WindowImplWin*>(window_->getImpl())->getHandle();

        if (!startup()) {
            return false;
        }

        close_event_ = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!close_event_) {
            return false;
        }

        HRESULT hr = ::MFCreateMediaSession(nullptr, &media_session_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create Media Session: " << hr;
            return false;
        }

        hr = ::MFCreateSourceResolver(&source_resolver_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create source resolver: " << hr;
            return false;
        }

        create_callback_ = new MFAsyncCallback(
            std::bind(&MediaPlayerWin::onCreateMediaSourceComplete, this, std::placeholders::_1));

        hr = source_resolver_->BeginCreateObjectFromURL(
            reinterpret_cast<const wchar_t*>(url.c_str()),
            MF_RESOLUTION_MEDIASOURCE, nullptr, &cancel_msc_cookie_, create_callback_, nullptr);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create media source: " << hr;
            return hr;
        }

        return true;
    }

    bool MediaPlayerWin::openFile(const std::u16string& file_name, Window* w) {
        if (state_ != State::Idle) {
            return false;
        }

        window_ = w;
        hwnd_ = static_cast<WindowImplWin*>(window_->getImpl())->getHandle();

        if (!startup()) {
            return false;
        }

        close_event_ = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!close_event_) {
            return false;
        }

        HRESULT hr = ::MFCreateMediaSession(nullptr, &media_session_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create Media Session: " << hr;
            return false;
        }

        hr = ::MFCreateSourceResolver(&source_resolver_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create source resolver: " << hr;
            return false;
        }

        rf_callback_ = new MFAsyncCallback(
            std::bind(&MediaPlayerWin::onReadFileComplete, this, std::placeholders::_1));

        hr = ::MFBeginCreateFile(
            MF_ACCESSMODE_READ,
            MF_OPENMODE_FAIL_IF_NOT_EXIST,
            MF_FILEFLAGS_NONE,
            reinterpret_cast<const wchar_t*>(file_name.c_str()),
            rf_callback_, nullptr, &cancel_rf_cookie_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to open media file: " << hr;
            return false;
        }

        return true;
    }

    bool MediaPlayerWin::start(bool current, int64_t position) {
        if (!media_session_) {
            return false;
        }

        PROPVARIANT prop;
        PropVariantInit(&prop);
        if (current) {
            prop.vt = VT_EMPTY;
        } else {
            prop.vt = VT_I8;
            prop.hVal.QuadPart = position * 10; // 100-nanosecond unit
        }
        HRESULT hr = media_session_->Start(nullptr, &prop);
        return SUCCEEDED(hr);
    }

    bool MediaPlayerWin::pause() {
        if (!media_session_) {
            return false;
        }

        HRESULT hr = media_session_->Pause();
        return SUCCEEDED(hr);
    }

    bool MediaPlayerWin::stop() {
        if (!media_session_) {
            return false;
        }

        HRESULT hr = media_session_->Stop();
        return SUCCEEDED(hr);
    }

    void MediaPlayerWin::close() {
        if (media_session_) {
            HRESULT hr = media_session_->Close();
            if (SUCCEEDED(hr) && state_ > State::Opened && state_ < State::Closed) {
                ::WaitForSingleObject(close_event_, INFINITE);
            }
        }

        destroy();
    }

    void MediaPlayerWin::destroy() {
        if (vp_activate_) {
            vp_activate_->Release();
            vp_activate_ = nullptr;
        }

        video_bitmap_.reset();
        video_texture_.reset();

        if (video_sample_) {
            ComPtr<IMFTrackedSample> track;
            HRESULT hr = video_sample_->QueryInterface(&track);
            if (SUCCEEDED(hr)) {
                track->SetAllocator(nullptr, nullptr);
            }
            video_sample_.reset();
        }

        if (media_session_) {
            media_session_->Shutdown();
            media_session_->Release();
            media_session_ = nullptr;
        }

        if (event_callback_) {
            event_callback_->Release();
            event_callback_ = nullptr;
        }
        if (create_callback_) {
            create_callback_->Release();
            create_callback_ = nullptr;
        }
        if (media_source_) {
            media_source_->Shutdown();
            media_source_->Release();
            media_source_ = nullptr;
        }

        if (cancel_msc_cookie_) {
            source_resolver_->CancelObjectCreation(cancel_msc_cookie_);
            cancel_msc_cookie_->Release();
            cancel_msc_cookie_ = nullptr;
        }

        if (source_resolver_) {
            source_resolver_->Release();
            source_resolver_ = nullptr;
        }

        if (cancel_rf_cookie_) {
            ::MFCancelCreateFile(cancel_rf_cookie_);
            cancel_rf_cookie_->Release();
            cancel_rf_cookie_ = nullptr;
        }

        if (rf_callback_) {
            rf_callback_->Release();
            rf_callback_ = nullptr;
        }

        shutdown();
        state_ = State::Idle;
    }

    void MediaPlayerWin::onHandleMessage(const utl::Message& msg) {
        switch (msg.id) {
        case MSG_OPEN_SUCCEEDED:
            state_ = State::Opened;
            if (callback_) {
                callback_->onMediaOpenComplete(true);
            }
            break;
        case MSG_OPEN_FAILED:
            if (callback_) {
                callback_->onMediaOpenComplete(false);
            }
            break;
        case MSG_MEDIA_STARTED:
            state_ = State::Started;
            if (callback_) {
                callback_->onMediaStarted();
            }
            break;
        case MSG_MEDIA_PAUSED:
            state_ = State::Paused;
            if (callback_) {
                callback_->onMediaPaused();
            }
            break;
        case MSG_MEDIA_STOPPED:
            state_ = State::Stopped;
            if (callback_) {
                callback_->onMediaStopped();
            }
            break;
        case MSG_MEDIA_ENDED:
            state_ = State::Ended;
            if (callback_) {
                callback_->onMediaEnded();
            }
            break;
        case MSG_MEDIA_CLOSED:
            state_ = State::Closed;
            if (callback_) {
                callback_->onMediaClosed();
            }
            break;
        case MSG_RENDER_SURFACE:
            if (callback_) {
                callback_->onRenderVideoFrame(static_cast<ImageFrame*>(msg.data));
            } else {
                delete static_cast<ImageFrame*>(msg.data);
            }
            break;
        default:
            break;
        }
    }

    void MediaPlayerWin::onCreateSurface(HANDLE shared) {
        auto device = static_cast<GPUDeviceD3D*>(
            Application::getGraphicDeviceManager()->getGPUDevice());

        HRESULT hr = device->getNative()->OpenSharedResource(shared, IID_PPV_ARGS(&video_texture_));
        if (SUCCEEDED(hr)) {
            ComPtr<IDXGISurface> dxgi_surface;
            hr = video_texture_->QueryInterface(&dxgi_surface);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to query DXGI surface: " << hr;
                return;
            }

            auto props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));

            DXGI_SURFACE_DESC desc;
            dxgi_surface->GetDesc(&desc);

            hr = static_cast<CyroRenderTargetD2D*>(
                window_->getCanvas()->getBuffer()->getRT())->getNative()->CreateSharedBitmap(
                __uuidof(IDXGISurface), dxgi_surface.get(), &props, &video_bitmap_);
            if (FAILED(hr)) {
                return;
            }
        }
    }

    void MediaPlayerWin::onDestroySurface() {
        video_bitmap_.reset();
        video_texture_.reset();
        video_sample_.reset();
    }

    void MediaPlayerWin::onRenderSurface(IMFSample* sample) {
        //sample->AddRef();
        //video_sample_ = sample;

        auto rt = static_cast<CyroRenderTargetD2D*>(
            window_->getCanvas()->getBuffer()->getRT())->getNative();
        auto context = static_cast<GPUContextD3D*>(
            Application::getGraphicDeviceManager()->getGPUContext())->getNative();

        ImageFrameWin* frame = new ImageFrameWin(video_bitmap_, rt, context, video_texture_);
        utl::Message msg;
        msg.id = MSG_RENDER_SURFACE;
        msg.data = frame;
        cycler_.post(&msg);
    }

    HRESULT MediaPlayerWin::onReadFileComplete(IMFAsyncResult* result) {
        auto scope_obj = SCOPED_CONVERTER{
            cycler_.post(MSG_OPEN_FAILED);
        };

        ComPtr<IMFByteStream> stream;
        HRESULT hr = ::MFEndCreateFile(result, &stream);
        if (FAILED(hr)) {
            return hr;
        }

        create_callback_ = new MFAsyncCallback(
            std::bind(&MediaPlayerWin::onCreateMediaSourceComplete, this, std::placeholders::_1));

        hr = source_resolver_->BeginCreateObjectFromByteStream(
            stream.get(), nullptr,
            MF_RESOLUTION_MEDIASOURCE, nullptr, &cancel_msc_cookie_, create_callback_, nullptr);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create media source: " << hr;
            return hr;
        }

        scope_obj.cancel();
        return hr;
    }

    HRESULT MediaPlayerWin::onCreateMediaSourceComplete(IMFAsyncResult* result) {
        auto scope_obj = SCOPED_CONVERTER {
            cycler_.post(MSG_OPEN_FAILED);
        };

        MF_OBJECT_TYPE type;
        IUnknown* object;
        HRESULT hr = source_resolver_->EndCreateObjectFromByteStream(result, &type, &object);
        if (FAILED(hr)) {
            return hr;
        }

        if (type != MF_OBJECT_MEDIASOURCE) {
            return hr;
        }

        hr = object->QueryInterface(IID_PPV_ARGS(&media_source_));
        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IMFPresentationDescriptor> desc;
        hr = media_source_->CreatePresentationDescriptor(&desc);
        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IMFTopology> topology;
        hr = createPlaybackTopology(media_source_, desc.get(), hwnd_, &topology);
        if (FAILED(hr)) {
            return hr;
        }

        auto callback = std::bind(
            &MediaPlayerWin::onEventReceived, this, std::placeholders::_1);
        event_callback_ = new MFAsyncCallback(callback);

        hr = media_session_->BeginGetEvent(event_callback_, nullptr);
        if (FAILED(hr)) {
            return hr;
        }

        hr = media_session_->SetTopology(MFSESSION_SETTOPOLOGY_IMMEDIATE, topology.get());
        if (FAILED(hr)) {
            return hr;
        }

        scope_obj.cancel();
        cycler_.post(MSG_OPEN_SUCCEEDED);
        return hr;
    }

    HRESULT MediaPlayerWin::onEventReceived(IMFAsyncResult* result) {
        ComPtr<IMFMediaEvent> ev;
        HRESULT hr = media_session_->EndGetEvent(result, &ev);
        if (FAILED(hr)) {
            return hr;
        }

        MediaEventType type;
        hr = ev->GetType(&type);
        if (FAILED(hr)) {
            return hr;
        }

        switch (type) {
        case MESessionStarted:
            cycler_.post(MSG_MEDIA_STARTED);
            break;
        case MESessionPaused:
            cycler_.post(MSG_MEDIA_PAUSED);
            break;
        case MESessionStopped:
            cycler_.post(MSG_MEDIA_STOPPED);
            break;
        case MESessionEnded:
            cycler_.post(MSG_MEDIA_ENDED);
            break;
        case MESessionClosed:
            cycler_.post(MSG_MEDIA_CLOSED);
            if (close_event_) {
                ::SetEvent(close_event_);
            }
            return hr;

        default:
            break;
        }

        return media_session_->BeginGetEvent(event_callback_, nullptr);
    }

    HRESULT MediaPlayerWin::createPlaybackTopology(
        IMFMediaSource* source,
        IMFPresentationDescriptor* desc,
        HWND hwnd, IMFTopology** out_topology)
    {
        ComPtr<IMFTopology> topology;
        HRESULT hr = ::MFCreateTopology(&topology);
        if (FAILED(hr)) {
            return hr;
        }

        DWORD stream_desc_count;
        hr = desc->GetStreamDescriptorCount(&stream_desc_count);
        if (FAILED(hr)) {
            return hr;
        }

        for (DWORD i = 0; i < stream_desc_count; ++i) {
            hr = addBranchToPartialTopology(topology.get(), source, desc, i, hwnd);
            if (FAILED(hr)) {
                return hr;
            }
        }

        *out_topology = topology.detach();
        return S_OK;
    }

    HRESULT MediaPlayerWin::addBranchToPartialTopology(
        IMFTopology* topology,
        IMFMediaSource* source,
        IMFPresentationDescriptor* desc,
        DWORD stream_index, HWND hwnd)
    {
        BOOL is_selected;
        ComPtr<IMFStreamDescriptor> stream_desc;
        HRESULT hr = desc->GetStreamDescriptorByIndex(stream_index, &is_selected, &stream_desc);
        if (FAILED(hr)) {
            return hr;
        }

        if (!is_selected) {
            return S_OK;
        }

        ComPtr<IMFActivate> sink_node;
        hr = createMediaSinkActivate(stream_desc.get(), hwnd, &sink_node);
        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IMFTopologyNode> source_node;
        hr = addSourceNode(topology, source, desc, stream_desc.get(), &source_node);
        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IMFTopologyNode> output_node;
        hr = addOutputNode(topology, sink_node.get(), 0, &output_node);
        if (FAILED(hr)) {
            return hr;
        }

        hr = source_node->ConnectOutput(0, output_node.get(), 0);
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }

    HRESULT MediaPlayerWin::createMediaSinkActivate(
        IMFStreamDescriptor* desc, HWND hwnd, IMFActivate** out)
    {
        ComPtr<IMFMediaTypeHandler> handler;
        ComPtr<IMFActivate> activate;

        HRESULT hr = desc->GetMediaTypeHandler(&handler);
        if (FAILED(hr)) {
            return hr;
        }

        GUID major_type;
        hr = handler->GetMajorType(&major_type);
        if (FAILED(hr)) {
            return hr;
        }

        if (major_type == MFMediaType_Audio) {
            hr = ::MFCreateAudioRendererActivate(&activate);
        } else if (major_type == MFMediaType_Video) {
            hr = ::MFCreateVideoRendererActivate(hwnd, &activate);
            if (SUCCEEDED(hr)) {
                hr = activate->SetUnknown(MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_ACTIVATE, vp_activate_);
            }
        } else {
            hr = E_FAIL;
        }

        if (FAILED(hr)) {
            return hr;
        }

        *out = activate.detach();
        return S_OK;
    }

    HRESULT MediaPlayerWin::addSourceNode(
        IMFTopology* topology,
        IMFMediaSource* source,
        IMFPresentationDescriptor* pd,
        IMFStreamDescriptor* sd, IMFTopologyNode** out_node)
    {
        ComPtr<IMFTopologyNode> node;
        HRESULT hr = ::MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &node);
        if (FAILED(hr)) {
            return hr;
        }

        hr = node->SetUnknown(MF_TOPONODE_SOURCE, source);
        if (FAILED(hr)) {
            return hr;
        }

        hr = node->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pd);
        if (FAILED(hr)) {
            return hr;
        }

        hr = node->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, sd);
        if (FAILED(hr)) {
            return hr;
        }

        hr = topology->AddNode(node.get());
        if (FAILED(hr)) {
            return hr;
        }

        *out_node = node.detach();
        return S_OK;
    }

    HRESULT MediaPlayerWin::addOutputNode(
        IMFTopology* topology, IMFActivate* activate, DWORD id, IMFTopologyNode** out_node)
    {
        ComPtr<IMFTopologyNode> node;
        HRESULT hr = ::MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &node);
        if (FAILED(hr)) {
            return hr;
        }

        hr = node->SetObject(activate);
        if (FAILED(hr)) {
            return hr;
        }

        hr = node->SetUINT32(MF_TOPONODE_STREAMID, id);
        if (FAILED(hr)) {
            return hr;
        }

        hr = node->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, TRUE);
        if (FAILED(hr)) {
            return hr;
        }

        hr = topology->AddNode(node.get());
        if (FAILED(hr)) {
            return hr;
        }

        *out_node = node.detach();
        return S_OK;
    }

}
