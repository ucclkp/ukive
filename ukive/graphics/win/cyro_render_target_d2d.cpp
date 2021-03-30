// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/cyro_render_target_d2d.h"

#include "utils/log.h"

#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/graphics/win/gpu/gpu_texture_d3d.h"


namespace ukive {

    ImageFrame* CyroRenderTargetD2D::createSharedImageFrame(
        GPUTexture* texture, const ImageOptions& options)
    {
        ID3D11Resource* res = nullptr;
        auto& desc = texture->getDesc();
        switch (desc.dim) {
        case GPUTexture::Dimension::_1D:
            res = static_cast<GPUTexture1DD3D*>(texture)->getNative();
            break;
        case GPUTexture::Dimension::_2D:
            res = static_cast<GPUTexture2DD3D*>(texture)->getNative();
            break;
        case GPUTexture::Dimension::_3D:
            res = static_cast<GPUTexture3DD3D*>(texture)->getNative();
            break;
        }

        if (!res) {
            return nullptr;
        }

        ComPtr<IDXGISurface> dxgi_surface;
        HRESULT hr = res->QueryInterface(&dxgi_surface);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to query DXGI surface: " << hr;
            return nullptr;
        }

        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(options);

        ComPtr<ID2D1Bitmap> d2d_bmp;
        hr = rt_->CreateSharedBitmap(
            __uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &d2d_bmp);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return nullptr;
        }

        return new ImageFrameWin(d2d_bmp);
    }

    void CyroRenderTargetD2D::destroy() {
        rt_.reset();
    }

    void CyroRenderTargetD2D::setNative(const ComPtr<ID2D1RenderTarget>& rt) {
        rt_ = rt;
    }

    ComPtr<ID2D1RenderTarget> CyroRenderTargetD2D::getNative() const {
        return rt_;
    }

}
