// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/cyro_render_target_d2d.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_context_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_texture_d3d11.h"


namespace ukive {
namespace win {

    GPtr<ImageFrame> CyroRenderTargetD2D::createSharedImageFrame(
        const GPtr<GPUTexture>& texture, const ImageOptions& options)
    {
        auto& desc = texture->getDesc();
        if (desc.dim != GPUTexture::Dimension::_2D) {
            return {};
        }

        auto res = texture.cast<GPUTexture2DD3D11>()->getNative();
        if (!res) {
            return {};
        }

        utl::win::ComPtr<IDXGISurface> dxgi_surface;
        HRESULT hr = res->QueryInterface(&dxgi_surface);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to query DXGI surface: " << hr;
            return {};
        }

        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(options);

        utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
        hr = rt_->CreateSharedBitmap(
            __uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &d2d_bmp);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return {};
        }

        return GPtr<ImageFrame>(new ImageFrameWin(options, {}, {}, d2d_bmp));
    }

    void CyroRenderTargetD2D::destroy() {
        rt_.reset();
    }

    void CyroRenderTargetD2D::setNative(const utl::win::ComPtr<ID2D1RenderTarget>& rt) {
        rt_ = rt;
    }

    utl::win::ComPtr<ID2D1RenderTarget> CyroRenderTargetD2D::getNative() const {
        return rt_;
    }

}
}