// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MEDIA_PLAYER_WIN_H_
#define UKIVE_MEDIA_WIN_MEDIA_PLAYER_WIN_H_

#include <string>

#include "utils/message/cycler.h"

#include "ukive/media/media_player.h"

#include <d2d1.h>
#include <d3d11.h>
#include <mfidl.h>

#include "ukive/media/win/mf_d3d9_render_engine.h"


namespace ukive {

    class Window;
    class MFAsyncCallback;
    class MFVideoPresenterActivate;
    class ImageFrame;

    class MediaPlayerWin :
        public MediaPlayer,
        public MFRenderCallback,
        public utl::CyclerListener
    {
    public:
        MediaPlayerWin();
        ~MediaPlayerWin();

        void setCallback(MediaPlayerCallback* cb) override;
        void setDisplaySize(const Size& size) override;

        bool openUrl(const std::u16string& url, Window* w) override;
        bool openFile(const std::u16string& file_name, Window* w) override;

        bool start(bool current, int64_t position) override;
        bool pause() override;
        bool stop() override;
        void close() override;

    private:
        enum class State {
            Idle,
            Opened,
            Started,
            Paused,
            Stopped,
            Ended,
            Closed,
        };

        static bool startup();
        static void shutdown();
        static unsigned int startup_counter_;

        void destroy();

        // utl::CyclerListener
        void onHandleMessage(const utl::Message& msg) override;

        // MFRenderCallback
        void onCreateSurface(HANDLE shared) override;
        void onDestroySurface() override;
        void onRenderSurface(IMFSample* sample) override;

        HRESULT onReadFileComplete(IMFAsyncResult* result);
        HRESULT onCreateMediaSourceComplete(IMFAsyncResult* result);
        HRESULT onEventReceived(IMFAsyncResult* result);

        HRESULT createPlaybackTopology(
            IMFMediaSource* source,
            IMFPresentationDescriptor* desc,
            HWND hwnd, IMFTopology** out_topology);
        HRESULT addBranchToPartialTopology(
            IMFTopology* topology,
            IMFMediaSource* source,
            IMFPresentationDescriptor* desc,
            DWORD stream_index, HWND hwnd);
        HRESULT createMediaSinkActivate(
            IMFStreamDescriptor* desc, HWND hwnd, IMFActivate** out);
        HRESULT addSourceNode(
            IMFTopology* topology,
            IMFMediaSource* source,
            IMFPresentationDescriptor* pd,
            IMFStreamDescriptor* sd, IMFTopologyNode** out_node);
        HRESULT addOutputNode(
            IMFTopology* topology,
            IMFActivate* activate, DWORD id, IMFTopologyNode** out_node);

        IUnknown* cancel_rf_cookie_ = nullptr;
        MFAsyncCallback* rf_callback_ = nullptr;


        HANDLE close_event_ = nullptr;
        IMFMediaSession* media_session_ = nullptr;
        MFVideoPresenterActivate* vp_activate_ = nullptr;

        IMFSourceResolver* source_resolver_ = nullptr;
        IUnknown* cancel_msc_cookie_ = nullptr;
        MFAsyncCallback* create_callback_ = nullptr;

        IMFMediaSource* media_source_ = nullptr;
        MFAsyncCallback* event_callback_ = nullptr;

        HWND hwnd_ = nullptr;
        Window* window_ = nullptr;
        ComPtr<IMFSample> video_sample_;
        ComPtr<ID2D1Bitmap> video_bitmap_;
        ComPtr<ID3D11Texture2D> video_texture_;
        MediaPlayerCallback* callback_ = nullptr;

        utl::Cycler cycler_;
        State state_ = State::Idle;
    };

}

#endif  // UKIVE_MEDIA_WIN_MEDIA_PLAYER_WIN_H_