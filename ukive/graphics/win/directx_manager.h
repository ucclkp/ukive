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

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/system/win/com_ptr.hpp"


namespace ukive {

    class GPUDeviceD3D;
    class GPUContextD3D;
    class ImageOptions;

    class DirectXManager : public GraphicDeviceManager {
    public:
        DirectXManager();
        ~DirectXManager();

        bool initialize() override;
        bool recreate() override;
        void destroy() override;

        GPUDevice* getGPUDevice() const override;
        GPUContext* getGPUContext() const override;

        void enumSystemFonts();

        bool isAdapterChanged() const;

        ComPtr<ID2D1Factory> getD2DFactory() const;
        ComPtr<IDWriteFactory> getDWriteFactory() const;
        ComPtr<IDXGIFactory1> getDXGIFactory() const;

        ComPtr<IDXGIDevice> getDXGIDevice() const;
        ComPtr<ID3D11Device> getD3DDevice() const;
        ComPtr<ID3D11DeviceContext> getD3DDeviceContext() const;

        ComPtr<ID2D1RenderTarget> createWICRenderTarget(IWICBitmap* wic_bitmap);
        ComPtr<ID2D1DCRenderTarget> createDCRenderTarget(const ImageOptions& options);

        ComPtr<ID3D11Texture2D> createTexture2D(
            UINT width, UINT height, bool shader_res, bool hdr, bool gdi_compat);
        ComPtr<ID3D11Texture2D> createTexture2D(IWICBitmap* wic_bmp);
        ComPtr<ID2D1RenderTarget> createDXGIRenderTarget(
            IDXGISurface* surface, bool gdi_compat, const ImageOptions& options);

    private:
        bool initPersistance();
        void shutdownPersistance();

        bool initDevice();
        void shutdownDevice();

        ComPtr<ID2D1Factory> d2d_factory_;
        ComPtr<IDWriteFactory> dwrite_factory_;
        ComPtr<IDXGIFactory1> dxgi_factory_;

        ComPtr<ID3D11Device> d3d_device_;
        ComPtr<ID3D11DeviceContext> d3d_devicecontext_;

        ComPtr<IDXGIDevice> dxgi_device_;

        std::unique_ptr<GPUDeviceD3D> gpu_device_;
        std::unique_ptr<GPUContextD3D> gpu_context_;
    };

}

#endif  // UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
