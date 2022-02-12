// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_MEDIA_WIN_MF_D3D9_RENDER_ENGINE_H_
#define UKIVE_MEDIA_WIN_MF_D3D9_RENDER_ENGINE_H_

#include <d3d9.h>
#include <dxva2api.h>
#include <mfidl.h>

#include "utils/memory/win/com_ptr.hpp"
#include "utils/sync/win/critical_section.hpp"

#include "ukive/graphics/rect.hpp"
#include "ukive/media/win/mf_common.h"
#include "ukive/media/win/mf_sample_scheduler.h"


namespace ukive {
namespace win {

    class MFRenderCallback {
    public:
        virtual ~MFRenderCallback() = default;

        virtual void onCreateSurface(HANDLE shared) = 0;
        virtual void onDestroySurface() = 0;
        virtual void onRenderSurface(IMFSample* sample) = 0;
    };

    class MFD3D9RenderEngine :
        public MFSampleScheduler::Callback
    {
    public:
        enum class DevState {
            OK,
            Reset,
            Removed,
        };

        MFD3D9RenderEngine();
        ~MFD3D9RenderEngine();

        HRESULT initialize();

        HRESULT getService(REFGUID service, REFIID riid, void** ppv);
        HRESULT checkFormat(D3DFORMAT format);

        void setRenderCallback(MFRenderCallback* cb);

        HRESULT setVideoWindow(HWND window);
        HWND getVideoWindow() const;

        void setDestinationRect(const Rect& rect);
        Rect getDestinationRect() const;

        HRESULT createVideoSamples(IMFMediaType* format, SampleList& sample_queue);
        void destroyResources();

        HRESULT checkDeviceState(DevState* state);
        HRESULT presentSample(IMFSample* sample, LONGLONG target) override;

        UINT getRefreshRate() const;

    private:
        HRESULT initializeD3D9();
        HRESULT createD3D9Device();
        HRESULT createD3D9Sample(IDirect3DSwapChain9* swapchain, IMFSample** sample);
        void updateDestinationRect();
        static bool findAdapter(IDirect3D9* d3d9, HMONITOR monitor, UINT* found);

        UINT dev_reset_token_ = 0;
        Rect dest_rect_;
        D3DDISPLAYMODE display_mode_;
        utl::win::CritSec crit_sec_;
        HWND window_ = nullptr;

        IDirect3D9Ex* d3d9_ = nullptr;
        IDirect3DDevice9Ex* device_ = nullptr;
        IDirect3DDeviceManager9* device_mgr_ = nullptr;
        utl::win::ComPtr<IDirect3DSurface9> surface_;

        HANDLE shared_handle_ = nullptr;
        utl::win::ComPtr<IDirect3DTexture9> interop_texture_;
        MFRenderCallback* render_callback_ = nullptr;
    };

}
}

#endif  // UKIVE_MEDIA_WIN_MF_D3D9_RENDER_ENGINE_H_