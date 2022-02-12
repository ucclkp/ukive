// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/directx_manager.h"

#include <wincodec.h>

#include "utils/log.h"

#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/gpu/gpu_context_d3d.h"
#include "ukive/graphics/win/gpu/gpu_device_d3d.h"
#include "ukive/window/window.h"


namespace ukive {

    void configD2DRTProps(
        D2D1_RENDER_TARGET_PROPERTIES* props, const ImageOptions& options)
    {
        DXGI_FORMAT format;
        switch (options.pixel_format) {
        case ImagePixelFormat::HDR:
            format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case ImagePixelFormat::RAW:
            format = DXGI_FORMAT_UNKNOWN;
            break;
        case ImagePixelFormat::B8G8R8A8_UNORM:
        default:
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        }

        D2D1_ALPHA_MODE alpha_mode;
        switch (options.alpha_mode) {
        case ImageAlphaMode::STRAIGHT:
            alpha_mode = D2D1_ALPHA_MODE_STRAIGHT;
            break;
        case ImageAlphaMode::IGNORED:
            alpha_mode = D2D1_ALPHA_MODE_IGNORE;
            break;

        case ImageAlphaMode::PREMULTIPLIED:
        default:
            alpha_mode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            break;
        }

        props->pixelFormat.format = format;
        props->pixelFormat.alphaMode = alpha_mode;

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
        return initDevice();
    }

    bool DirectXManager::recreate() {
        HRESULT hr = d3d_device_->GetDeviceRemovedReason();
        if (SUCCEEDED(hr)) {
            return true;
        }

        shutdownDevice();
        return initDevice();
    }

    void DirectXManager::destroy() {
        shutdownPersistance();
        shutdownDevice();
    }

    GPUDevice* DirectXManager::getGPUDevice() const {
        return gpu_device_.get();
    }

    GPUContext* DirectXManager::getGPUContext() const {
        return gpu_context_.get();
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
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &d2d_factory_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d2d factory.";
            return false;
        }

        hr = ::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&dwrite_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dwrite factory.";
            return false;
        }

        hr = ::CreateDXGIFactory(
            __uuidof(IDXGIFactory),
            reinterpret_cast<void**>(&dxgi_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi factory.";
            return false;
        }

        return true;
    }

    void DirectXManager::shutdownPersistance() {
        d2d_factory_.reset();
        dwrite_factory_.reset();
        dxgi_factory_.reset();
    }

    bool DirectXManager::initDevice() {
        D3D_FEATURE_LEVEL featureLevel[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };


        //D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_WARP;
        //UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUGGABLE;

        D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_HARDWARE;
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        HRESULT hr = ::D3D11CreateDevice(
            nullptr, driver_type, nullptr, flags,
            featureLevel, ARRAYSIZE(featureLevel), D3D11_SDK_VERSION,
            &d3d_device_, nullptr, &d3d_devicecontext_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d3d device.";
            return false;
        }

        hr = d3d_device_->QueryInterface(&dxgi_device_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi device.";
            return false;
        }

        gpu_device_ = std::make_unique<GPUDeviceD3D>(d3d_device_);
        gpu_context_ = std::make_unique<GPUContextD3D>(d3d_devicecontext_);

        return true;
    }

    void DirectXManager::shutdownDevice() {
        d3d_devicecontext_.reset();
        d3d_device_.reset();

        dxgi_device_.reset();
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

        return render_target;
    }

}
}