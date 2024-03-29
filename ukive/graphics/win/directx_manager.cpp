// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/directx_manager.h"

#include <wincodec.h>

#include "utils/log.h"

#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_context_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_device_d3d11.h"
#include "ukive/graphics/win/images/image_options_win_utils.h"
#include "ukive/window/window.h"


namespace ukive {

    void configD2DRTProps(
        D2D1_RENDER_TARGET_PROPERTIES* props, const ImageOptions& options)
    {
        props->pixelFormat.format = win::mapDXGIFormat(options.pixel_format);
        props->pixelFormat.alphaMode = win::mapD2D1AlphaMode(options.alpha_mode);

        switch (options.dpi_type) {
        case ImageDPIType::SPECIFIED:
            props->dpiX = options.dpi_x;
            props->dpiY = options.dpi_y;
            break;

        case ImageDPIType::DEFAULT:
        default:
            break;
        }
    }

}

namespace ukive {
namespace win {

    DirectXManager::DirectXManager() {}
    DirectXManager::~DirectXManager() {}

    bool DirectXManager::initialize() {
        if (!initPersistance()) {
            return false;
        }

        if (!initDevice()) {
            return false;
        }

        return true;
    }

    void DirectXManager::destroy() {
        shutdownDevice();
        shutdownPersistance();
    }

    bool DirectXManager::recreate() {
        std::lock_guard<std::mutex> lg(devc_sync_);

        HRESULT hr = d3d_device_->GetDeviceRemovedReason();
        if (SUCCEEDED(hr)) {
            return true;
        }

        notifyDeviceLost();
        demolish();

        shutdownDevice();

        /**
         * 确保 demolish / rebuild 成对调用，
         * 即使创建设备失败。
         */
        bool ret = initDevice();

        rebuild(ret);
        notifyDeviceRestored();
        return ret;
    }

    GPtr<GPUDevice> DirectXManager::getGPUDevice() const {
        return gpu_device_;
    }

    GPtr<GPUContext> DirectXManager::getGPUContext() const {
        return gpu_context_;
    }

    void DirectXManager::enumSystemFonts() {
        utl::win::ComPtr<IDWriteFontCollection> collection;
        HRESULT hr = dwrite_factory_->GetSystemFontCollection(&collection);
        if (FAILED(hr)) {
            return;
        }

        auto count = collection->GetFontFamilyCount();
        for (UINT32 i = 0; i < count; ++i) {
            utl::win::ComPtr<IDWriteFontFamily> family;
            hr = collection->GetFontFamily(i, &family);
            if (FAILED(hr)) {
                continue;
            }

            utl::win::ComPtr<IDWriteLocalizedStrings> strings;
            hr = family->GetFamilyNames(&strings);
            if (FAILED(hr)) {
                continue;
            }

            auto font_count = strings->GetCount();
            for (UINT32 j = 0; j < font_count; ++j) {
                UINT32 length;
                hr = strings->GetStringLength(j, &length);
                if (SUCCEEDED(hr)) {
                    WCHAR* buffer = new WCHAR[length + 1];
                    hr = strings->GetString(j, buffer, length + 1);
                    if (SUCCEEDED(hr)) {
                        ::OutputDebugStringW(buffer);
                        ::OutputDebugStringW(L"\n");
                    }

                    delete[] buffer;
                }
            }
        }
    }

    bool DirectXManager::isAdapterChanged() const {
        return dxgi_factory_->IsCurrent() == FALSE;
    }

    bool DirectXManager::initPersistance() {
        HRESULT hr = ::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED,
            &d2d_factory_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d2d factory.";
            return false;
        }

        utl::win::ComPtr<IUnknown> unk;
        hr = ::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory), &unk);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dwrite factory.";
            return false;
        }

        dwrite_factory_ = unk.cast<IDWriteFactory>();
        if (!dwrite_factory_) {
            LOG(Log::ERR) << "Failed to create dwrite factory.";
            return false;
        }

        return true;
    }

    void DirectXManager::shutdownPersistance() {
        d2d_factory_.reset();
        dwrite_factory_.reset();
    }

    bool DirectXManager::chooseAdapter(UINT idx, IDXGIAdapter1** adapter) {
        HRESULT hr;
        utl::win::ComPtr<IDXGIFactory1> dxgi_factory;
        hr = ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi factory.";
            return false;
        }

        hr = dxgi_factory->EnumAdapters1(0, adapter);
        if (SUCCEEDED(hr)) {
            DXGI_ADAPTER_DESC1 ada_desc;
            hr = (*adapter)->GetDesc1(&ada_desc);
            if (SUCCEEDED(hr)) {
            }
        } else {
            LOG(Log::WARNING) << "Failed to get adapter.";
        }

        return true;
    }

    bool DirectXManager::initDevice() {
        D3D_FEATURE_LEVEL feature_levels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        D3D_DRIVER_TYPE driver_type;
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        utl::win::ComPtr<IDXGIAdapter1> adapter;
        if (true) {
            if (chooseAdapter(0, &adapter)) {
                driver_type = D3D_DRIVER_TYPE_UNKNOWN;
            } else {
                driver_type = D3D_DRIVER_TYPE_HARDWARE;
            }
        } else {
            driver_type = D3D_DRIVER_TYPE_WARP;
            flags |= D3D11_CREATE_DEVICE_DEBUGGABLE;
        }

        HRESULT hr = ::D3D11CreateDevice(
            adapter.get(), driver_type, nullptr, flags,
            feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
            &d3d_device_, nullptr, &d3d_devicecontext_);
        if (FAILED(hr)) {
            hr = ::D3D11CreateDevice(
                adapter.get(), driver_type, nullptr, flags,
                feature_levels + 1, ARRAYSIZE(feature_levels) - 1, D3D11_SDK_VERSION,
                &d3d_device_, nullptr, &d3d_devicecontext_);
            if (FAILED(hr)) {
                LOG(Log::ERR) << "Failed to create d3d device.";
                return false;
            }
        }

        hr = d3d_device_->QueryInterface(&dxgi_device_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create DXGI device.";
            return false;
        }

        utl::win::ComPtr<IDXGIAdapter1> _adapter;
        hr = dxgi_device_->GetParent(IID_PPV_ARGS(&_adapter));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to get DXGI adapter from DXGI device.";
            return false;
        }

        utl::win::ComPtr<IDXGIFactory1> _dxgi_factory;
        hr = _adapter->GetParent(IID_PPV_ARGS(&_dxgi_factory));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to get DXGI factory from DXGI adapter.";
            return false;
        }
        dxgi_factory_ = _dxgi_factory;

        gpu_device_ = new GPUDeviceD3D11(d3d_device_);
        gpu_context_ = new GPUContextD3D11(d3d_devicecontext_);

        return true;
    }

    void DirectXManager::shutdownDevice() {
        gpu_device_.reset();
        gpu_context_.reset();
        d3d_devicecontext_.reset();
        d3d_device_.reset();
        dxgi_device_.reset();
        dxgi_factory_.reset();
    }

    utl::win::ComPtr<ID2D1Factory> DirectXManager::getD2DFactory() const {
        return d2d_factory_;
    }

    utl::win::ComPtr<IDWriteFactory> DirectXManager::getDWriteFactory() const {
        return dwrite_factory_;
    }

    utl::win::ComPtr<IDXGIFactory1> DirectXManager::getDXGIFactory() const {
        return dxgi_factory_;
    }

    utl::win::ComPtr<IDXGIDevice> DirectXManager::getDXGIDevice() const {
        return dxgi_device_;
    }

    utl::win::ComPtr<ID3D11Device> DirectXManager::getD3DDevice() const {
        return d3d_device_;
    }

    utl::win::ComPtr<ID3D11DeviceContext> DirectXManager::getD3DDeviceContext() const {
        return d3d_devicecontext_;
    }

    utl::win::ComPtr<ID2D1RenderTarget> DirectXManager::createWICRenderTarget(IWICBitmap* wic_bitmap) {
        utl::win::ComPtr<ID2D1RenderTarget> render_target;
        auto d2d_factory = getD2DFactory();
        if (!d2d_factory) {
            return {};
        }

        const D2D1_RENDER_TARGET_PROPERTIES properties
            = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE);

        HRESULT hr = d2d_factory->CreateWicBitmapRenderTarget(
            wic_bitmap, properties, &render_target);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create WICBitmap RenderTarget: " << hr;
            return {};
        }

        return render_target;
    }

    utl::win::ComPtr<ID2D1DCRenderTarget> DirectXManager::createDCRenderTarget(const ImageOptions& options) {
        utl::win::ComPtr<ID2D1DCRenderTarget> render_target;
        auto d2d_factory = getD2DFactory();
        if (!d2d_factory) {
            return {};
        }

        D2D1_RENDER_TARGET_PROPERTIES properties =
            D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE);

        configD2DRTProps(&properties, options);

        HRESULT hr = d2d_factory->CreateDCRenderTarget(
            &properties, &render_target);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create DC RenderTarget: " << hr;
            return {};
        }

        return render_target;
    }

    utl::win::ComPtr<ID3D11Texture2D> DirectXManager::createTexture2D(
        UINT width, UINT height, bool shader_res, bool hdr, bool gdi_compat)
    {
        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | (shader_res ? D3D11_BIND_SHADER_RESOURCE : 0);
        tex_desc.Format = hdr ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = gdi_compat ? D3D11_RESOURCE_MISC_GDI_COMPATIBLE : 0;

        utl::win::ComPtr<ID3D11Texture2D> tex;
        HRESULT hr = d3d_device_->CreateTexture2D(&tex_desc, nullptr, &tex);
        if (FAILED(hr)) {
            if (hr != DXGI_ERROR_DEVICE_REMOVED &&
                hr != DXGI_ERROR_DEVICE_RESET)
            {
                LOG(Log::WARNING) << "Failed to create 2d texture.";
            }
            return {};
        }
        return tex;
    }

    utl::win::ComPtr<ID3D11Texture2D> DirectXManager::createTexture2D(IWICBitmap* wic_bmp) {
        UINT width, height;
        HRESULT hr = wic_bmp->GetSize(&width, &height);
        if (FAILED(hr)) {
            return {};
        }

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = 0;

        WICRect lock_rect = { 0, 0, INT(width), INT(height) };
        utl::win::ComPtr<IWICBitmapLock> lock;
        hr = wic_bmp->Lock(&lock_rect, WICBitmapLockRead, &lock);
        if (FAILED(hr)) {
            return {};
        }

        UINT buf_size;
        BYTE* buf;
        hr = lock->GetDataPointer(&buf_size, &buf);
        if (FAILED(hr)) {
            return {};
        }

        UINT stride;
        hr = lock->GetStride(&stride);
        if (FAILED(hr)) {
            return {};
        }

        D3D11_SUBRESOURCE_DATA tex_data;
        tex_data.SysMemPitch = stride;
        tex_data.SysMemSlicePitch = 0;
        tex_data.pSysMem = buf;

        utl::win::ComPtr<ID3D11Texture2D> tex;
        hr = d3d_device_->CreateTexture2D(&tex_desc, &tex_data, &tex);
        if (FAILED(hr)) {
            if (hr != DXGI_ERROR_DEVICE_REMOVED &&
                hr != DXGI_ERROR_DEVICE_RESET)
            {
                LOG(Log::WARNING) << "Failed to create 2d texture.";
            }
            return {};
        }

        return tex;
    }

    utl::win::ComPtr<ID2D1RenderTarget> DirectXManager::createDXGIRenderTarget(
        IDXGISurface* surface, bool gdi_compat, const ImageOptions& options)
    {
        utl::win::ComPtr<ID2D1RenderTarget> render_target;
        auto d2d_factory = getD2DFactory();
        if (!d2d_factory) {
            return {};
        }

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(),
            0, 0, gdi_compat ? D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE : D2D1_RENDER_TARGET_USAGE_NONE);

        configD2DRTProps(&props, options);

        HRESULT hr = d2d_factory->CreateDxgiSurfaceRenderTarget(surface, props, &render_target);
        if (FAILED(hr)) {
            ubassert(false);
        }

        //render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        return render_target;
    }

}
}