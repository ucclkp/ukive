// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "mf_d3d9_render_engine.h"

#include "utils/log.h"
#include "utils/scope_utils.h"

#include <evr.h>
#include <Mferror.h>
#include <mfapi.h>


namespace ukive {

    const int OFFSCREEN_SURFACE_COUNT = 3;

    MFD3D9RenderEngine::MFD3D9RenderEngine() {
        ZeroMemory(&display_mode_, sizeof(display_mode_));
    }

    MFD3D9RenderEngine::~MFD3D9RenderEngine() {
        if (device_) {
            device_->Release();
        }
        if (surface_) {
            surface_->Release();
        }
        if (device_mgr_) {
            device_mgr_->Release();
        }
        if (d3d9_) {
            d3d9_->Release();
        }
    }

    HRESULT MFD3D9RenderEngine::initialize() {
        HRESULT hr = initializeD3D9();
        if (SUCCEEDED(hr)) {
            hr = createD3D9Device();
        }
        return hr;
    }

    HRESULT MFD3D9RenderEngine::getService(REFGUID service, REFIID riid, void** ppv) {
        if (!ppv) {
            return E_POINTER;
        }

        if (riid == __uuidof(IDirect3DDeviceManager9)) {
            if (!device_mgr_) {
                return MF_E_UNSUPPORTED_SERVICE;
            }

            *ppv = device_mgr_;
            device_mgr_->AddRef();
        } else {
            return MF_E_UNSUPPORTED_SERVICE;
        }

        return S_OK;
    }

    HRESULT MFD3D9RenderEngine::checkFormat(D3DFORMAT format) {
        HRESULT hr;
        UINT adapter;
        D3DDEVTYPE type;

        if (device_) {
            D3DDEVICE_CREATION_PARAMETERS params;
            hr = device_->GetCreationParameters(&params);
            if (FAILED(hr)) {
                return hr;
            }

            adapter = params.AdapterOrdinal;
            type = params.DeviceType;
        } else {
            adapter = D3DADAPTER_DEFAULT;
            type = D3DDEVTYPE_HAL;
        }

        D3DDISPLAYMODE mode;
        hr = d3d9_->GetAdapterDisplayMode(adapter, &mode);
        if (FAILED(hr)) {
            return hr;
        }

        hr = d3d9_->CheckDeviceType(adapter, type, mode.Format, format, TRUE);
        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

    void MFD3D9RenderEngine::setRenderCallback(MFRenderCallback* cb) {
        win::CritSecGuard guard(crit_sec_);
        render_callback_ = cb;
    }

    HRESULT MFD3D9RenderEngine::setVideoWindow(HWND window) {
        DCHECK(::IsWindow(window));
        DCHECK(window != window_);

        win::CritSecGuard guard(crit_sec_);

        window_ = window;
        updateDestinationRect();

        return createD3D9Device();
    }

    HWND MFD3D9RenderEngine::getVideoWindow() const {
        return window_;
    }

    void MFD3D9RenderEngine::setDestinationRect(const Rect& rect) {
        if (rect == dest_rect_) {
            return;
        }

        win::CritSecGuard guard(crit_sec_);

        dest_rect_ = rect;
        updateDestinationRect();
    }

    Rect MFD3D9RenderEngine::getDestinationRect() const {
        return dest_rect_;
    }

    HRESULT MFD3D9RenderEngine::createVideoSamples(
        IMFMediaType* format, SampleList& sample_queue)
    {
        if (!format) {
            return MF_E_UNEXPECTED;
        }

        win::CritSecGuard guard(crit_sec_);

        destroyResources();

        HRESULT hr;
        ESC_FROM_SCOPE {
            if (FAILED(hr)) {
                destroyResources();
            }
        };

        UINT32 width, height;
        hr = ::MFGetAttributeSize(format, MF_MT_FRAME_SIZE, &width, &height);
        if (FAILED(hr)) {
            return hr;
        }

        GUID sub_type_guid;
        hr = format->GetGUID(MF_MT_SUBTYPE, &sub_type_guid);
        if (FAILED(hr)) {
            return hr;
        }

        D3DPRESENT_PARAMETERS pp;
        ZeroMemory(&pp, sizeof(pp));

        pp.BackBufferWidth = width;
        pp.BackBufferHeight = height;
        pp.Windowed = TRUE;
        pp.SwapEffect = D3DSWAPEFFECT_COPY;
        pp.BackBufferFormat = D3DFORMAT(sub_type_guid.Data1);
        pp.hDeviceWindow = nullptr;
        pp.Flags = D3DPRESENTFLAG_VIDEO;
        pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

        for (int i = 0; i < OFFSCREEN_SURFACE_COUNT; ++i) {
            ComPtr<IDirect3DSwapChain9> swapchain;
            hr = device_->CreateAdditionalSwapChain(&pp, &swapchain);
            if (FAILED(hr)) {
                return hr;
            }

            ComPtr<IMFSample> sample;
            hr = createD3D9Sample(swapchain.get(), &sample);
            if (FAILED(hr)) {
                return hr;
            }

            sample_queue.push_back(sample);

            hr = sample->SetUnknown(MFSampleSwapChain, swapchain.get());
            if (FAILED(hr)) {
                return hr;
            }
        }

        hr = device_->CreateTexture(
            width, height, 1, D3DUSAGE_RENDERTARGET,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &interop_texture_, &shared_handle_);
        if (FAILED(hr)) {
            return hr;
        }

        if (render_callback_) {
            render_callback_->onCreateSurface(shared_handle_);
        }

        return hr;
    }

    void MFD3D9RenderEngine::destroyResources() {
        if (render_callback_) {
            render_callback_->onDestroySurface();
        }

        if (surface_) {
            surface_->Release();
            surface_ = nullptr;
        }

        interop_texture_.reset();
        shared_handle_ = nullptr;
    }

    HRESULT MFD3D9RenderEngine::checkDeviceState(DevState* state) {
        win::CritSecGuard guard(crit_sec_);

        HRESULT hr = device_->CheckDeviceState(window_);
        switch (hr) {
        case S_OK:
        case S_PRESENT_OCCLUDED:
        case S_PRESENT_MODE_CHANGED:
        case E_INVALIDARG:  // 参数无效，可能是窗口没了
            hr = S_OK;
            *state = DevState::OK;
            break;

        case D3DERR_DEVICELOST:
        case D3DERR_DEVICEHUNG:
            hr = createD3D9Device();
            if (FAILED(hr)) {
                return hr;
            }
            *state = DevState::Reset;
            hr = S_OK;
            break;

        case D3DERR_DEVICEREMOVED:
            *state = DevState::Removed;
            break;

        default:
            *state = DevState::OK;
            break;
        }
        return hr;
    }

    HRESULT MFD3D9RenderEngine::presentSample(IMFSample* sample, LONGLONG target) {
        HRESULT hr = S_OK;
        ComPtr<IMFMediaBuffer> buffer;
        ComPtr<IDirect3DSurface9> surface;
        if (sample) {
            hr = sample->GetBufferByIndex(0, &buffer);
            if (FAILED(hr)) {
                return hr;
            }

            hr = ::MFGetService(buffer.get(), MR_BUFFER_SERVICE, IID_PPV_ARGS(&surface));
            if (FAILED(hr)) {
                return hr;
            }
        } else if (surface_) {
            surface = surface_;
        }

        if (surface) {
            // https://docs.microsoft.com/en-us/windows/win32/direct3darticles/surface-sharing-between-windows-graphics-apis
            // https://docs.microsoft.com/en-us/windows/win32/api/d3d9helper/nf-d3d9helper-idirect3ddevice9-stretchrect
            // https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-opensharedresource
            ComPtr<IDirect3DSwapChain9> swapchain;
            hr = surface->GetContainer(IID_PPV_ARGS(&swapchain));
            if (FAILED(hr)) {
                return hr;
            }

            RECT dst_rect;
            dst_rect.left = dest_rect_.left;
            dst_rect.top = dest_rect_.top;
            dst_rect.right = dest_rect_.right;
            dst_rect.bottom = dest_rect_.bottom;

            hr = swapchain->Present(nullptr, &dst_rect, nullptr, nullptr, 0);
            if (SUCCEEDED(hr)) {
                ComPtr<IDirect3DSurface9> interop_surface;
                hr = interop_texture_->GetSurfaceLevel(0, &interop_surface);
                if (SUCCEEDED(hr)) {
                    hr = device_->StretchRect(
                        surface.get(), nullptr, interop_surface.get(), nullptr, D3DTEXF_NONE);

                    if (render_callback_ && sample) {
                        render_callback_->onRenderSurface(sample);
                    }
                }
            }

            surface_ = surface;
        }

        if (FAILED(hr)) {
            if (hr == D3DERR_DEVICELOST ||
                hr == D3DERR_DEVICENOTRESET||
                hr == D3DERR_DEVICEHUNG)
            {
                hr = S_OK;
            }
        }

        return hr;
    }

    UINT MFD3D9RenderEngine::getRefreshRate() const {
        return display_mode_.RefreshRate;
    }

    HRESULT MFD3D9RenderEngine::initializeD3D9() {
        DCHECK(!d3d9_);
        DCHECK(!device_mgr_);

        HRESULT hr = ::Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9_);
        if (FAILED(hr)) {
            return hr;
        }

        hr = DXVA2CreateDirect3DDeviceManager9(&dev_reset_token_, &device_mgr_);
        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

    HRESULT MFD3D9RenderEngine::createD3D9Device() {
        win::CritSecGuard guard(crit_sec_);

        if (!d3d9_ || !device_mgr_) {
            return MF_E_NOT_INITIALIZED;
        }

        D3DPRESENT_PARAMETERS pp;
        ZeroMemory(&pp, sizeof(pp));

        pp.BackBufferWidth = 1;
        pp.BackBufferHeight = 1;
        pp.Windowed = TRUE;
        pp.SwapEffect = D3DSWAPEFFECT_COPY;
        pp.BackBufferFormat = D3DFMT_UNKNOWN;
        pp.hDeviceWindow = ::GetDesktopWindow();
        pp.Flags = D3DPRESENTFLAG_VIDEO;
        pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

        UINT adapter = D3DADAPTER_DEFAULT;
        if (window_) {
            HMONITOR monitor = ::MonitorFromWindow(window_, MONITOR_DEFAULTTONEAREST);
            if (!findAdapter(d3d9_, monitor, &adapter)) {
                return E_FAIL;
            }
        }

        D3DCAPS9 caps;
        HRESULT hr = d3d9_->GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &caps);
        if (FAILED(hr)) {
            return hr;
        }

        DWORD vp;
        if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
            vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        } else {
            vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        ComPtr<IDirect3DDevice9Ex> device;
        hr = d3d9_->CreateDeviceEx(
            adapter, D3DDEVTYPE_HAL, pp.hDeviceWindow,
            vp | D3DCREATE_NOWINDOWCHANGES | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
            &pp, nullptr, &device);
        if (FAILED(hr)) {
            return hr;
        }

        hr = d3d9_->GetAdapterDisplayMode(adapter, &display_mode_);
        if (FAILED(hr)) {
            return hr;
        }

        hr = device_mgr_->ResetDevice(device.get(), dev_reset_token_);
        if (FAILED(hr)) {
            return hr;
        }

        if (device_) {
            device_->Release();
        }

        device_ = device.detach();
        return hr;
    }

    HRESULT MFD3D9RenderEngine::createD3D9Sample(IDirect3DSwapChain9* swapchain, IMFSample** sample) {
        ComPtr<IDirect3DSurface9> surface;
        HRESULT hr = swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
        if (FAILED(hr)) {
            return hr;
        }

        hr = device_->ColorFill(surface.get(), nullptr, D3DCOLOR_ARGB(0xFF, 0, 0, 0));
        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IMFSample> result_sample;
        hr = ::MFCreateVideoSampleFromSurface(surface.get(), &result_sample);
        if (FAILED(hr)) {
            return hr;
        }

        *sample = result_sample.detach();
        return hr;
    }

    void MFD3D9RenderEngine::updateDestinationRect() {
    }

    // static
    bool MFD3D9RenderEngine::findAdapter(IDirect3D9* d3d9, HMONITOR monitor, UINT* found) {
        UINT adapter_count = d3d9->GetAdapterCount();
        for (UINT i = 0; i < adapter_count; ++i) {
            HMONITOR tmp = d3d9->GetAdapterMonitor(i);
            if (!tmp) {
                break;
            }
            if (tmp == monitor) {
                *found = i;
                return true;
            }
        }
        return false;
    }

}
