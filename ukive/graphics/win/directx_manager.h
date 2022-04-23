// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
#define UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_

#include <memory>

#include <d2d1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <dxgi.h>

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_device.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class ImageOptions;

namespace win {

    class GPUDeviceD3D11;
    class GPUContextD3D11;

    class DirectXManager : public GraphicDeviceManager {
    public:
        DirectXManager();
        ~DirectXManager();

        bool initialize() override;
        bool recreate() override;
        void destroy() override;

        GPtr<GPUDevice> getGPUDevice() const override;
        GPtr<GPUContext> getGPUContext() const override;

        void enumSystemFonts();

        bool isAdapterChanged() const;

        utl::win::ComPtr<ID2D1Factory> getD2DFactory() const;
        utl::win::ComPtr<IDWriteFactory> getDWriteFactory() const;
        utl::win::ComPtr<IDXGIFactory1> getDXGIFactory() const;

        utl::win::ComPtr<IDXGIDevice> getDXGIDevice() const;
        utl::win::ComPtr<ID3D11Device> getD3DDevice() const;
        utl::win::ComPtr<ID3D11DeviceContext> getD3DDeviceContext() const;

        utl::win::ComPtr<ID2D1RenderTarget> createWICRenderTarget(IWICBitmap* wic_bitmap);
        utl::win::ComPtr<ID2D1DCRenderTarget> createDCRenderTarget(const ImageOptions& options);

        utl::win::ComPtr<ID3D11Texture2D> createTexture2D(
            UINT width, UINT height, bool shader_res, bool hdr, bool gdi_compat);
        utl::win::ComPtr<ID3D11Texture2D> createTexture2D(IWICBitmap* wic_bmp);
        utl::win::ComPtr<ID2D1RenderTarget> createDXGIRenderTarget(
            IDXGISurface* surface, bool gdi_compat, const ImageOptions& options);

    private:
        bool initPersistance();
        void shutdownPersistance();

        bool initDevice();
        void shutdownDevice();

        utl::win::ComPtr<ID2D1Factory> d2d_factory_;
        utl::win::ComPtr<IDWriteFactory> dwrite_factory_;
        utl::win::ComPtr<IDXGIFactory1> dxgi_factory_;

        utl::win::ComPtr<ID3D11Device> d3d_device_;
        utl::win::ComPtr<ID3D11DeviceContext> d3d_devicecontext_;

        utl::win::ComPtr<IDXGIDevice> dxgi_device_;

        GPtr<GPUDevice> gpu_device_;
        GPtr<GPUContext> gpu_context_;
    };

}
}

#endif  // UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
