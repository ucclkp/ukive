// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/offscreen_buffer_win.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/directx_manager.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_context_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_texture_d3d11.h"
#include "ukive/graphics/win/images/image_options_win_utils.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/window/window_dpi_utils.h"


namespace ukive {
namespace win {

    OffscreenBufferWin::OffscreenBufferWin()
        : width_(0), height_(0) {
    }

    bool OffscreenBufferWin::onCreate(
        int width, int height,
        const ImageOptions& options)
    {
        if (width <= 0 || height <= 0) {
            DLOG(Log::ERR) << "Invalid size value.";
            return false;
        }

        width_ = width;
        height_ = height;
        img_options_ = options;
        return createHardwareBRT(width_, height_);
    }

    GRet OffscreenBufferWin::onResize(int width, int height) {
        if (width <= 0 || height <= 0) {
            DLOG(Log::WARNING) << "Invalid size value.";
            return GRet::Succeeded;
        }

        if (nrt_.getNative() && width == width_ && height == height_) {
            return GRet::Succeeded;
        }

        width_ = width;
        height_ = height;
        return createHardwareBRT(width, height) ? GRet::Succeeded : GRet::Failed;
    }

    void OffscreenBufferWin::onDPIChange(float dpi_x, float dpi_y) {
        if (dpi_x <= 0 || dpi_y <= 0) {
            DLOG(Log::ERR) << "Invalid dpi values.";
            return;
        }

        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            img_options_.dpi_x = dpi_x;
            img_options_.dpi_y = dpi_y;
            nrt_.getNative()->SetDpi(dpi_x, dpi_y);
            break;
        default:
            break;
        }
    }

    void OffscreenBufferWin::onDestroy() {
        nrt_.destroy();
        d3d_tex2d_.reset();
    }

    void OffscreenBufferWin::onBeginDraw() {
        nrt_.getNative()->BeginDraw();
    }

    GRet OffscreenBufferWin::onEndDraw() {
        HRESULT hr = nrt_.getNative()->EndDraw();
        if (FAILED(hr)) {
            if (hr == D2DERR_RECREATE_TARGET) {
                if (DXMGR->recreate() && recreate()) {
                    return GRet::Retry;
                }
            }
            LOG(Log::ERR) << "Failed to draw d2d content: " << hr;
            return GRet::Failed;
        }
        return GRet::Succeeded;
    }

    GPtr<ImageFrame> OffscreenBufferWin::onExtractImage(const ImageOptions& options) {
        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(options);

        auto tex2d_d3d = d3d_tex2d_.cast<GPUTexture2DD3D11>()->getNative();

        utl::win::ComPtr<ID2D1Bitmap> bitmap;
        HRESULT hr = nrt_.getNative()->CreateSharedBitmap(
            __uuidof(IDXGISurface), tex2d_d3d.cast<IDXGISurface>().get(), &bmp_prop, &bitmap);
        if (FAILED(hr)) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED ||
                hr == DXGI_ERROR_DEVICE_RESET)
            {
                return {};
            }
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return {};
        }

        return GPtr<ImageFrame>(
            new ImageFrameWin(options, {}, {}, bitmap));
    }

    Size OffscreenBufferWin::getSize() const {
        return Size(width_, height_);
    }

    Size OffscreenBufferWin::getPixelSize() const {
        Size size;
        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            size.width(int(std::ceil(width_ * img_options_.dpi_x / kDefaultDpi)));
            size.height(int(std::ceil(height_ * img_options_.dpi_y / kDefaultDpi)));
            break;
        case ImageDPIType::DEFAULT:
        default:
            size.width(width_);
            size.height(height_);
            break;
        }
        return size;
    }

    const NativeRT* OffscreenBufferWin::getNativeRT() const {
        return &nrt_;
    }

    const ImageOptions& OffscreenBufferWin::getImageOptions() const {
        return img_options_;
    }

    GPtr<GPUTexture> OffscreenBufferWin::getTexture() const {
        return d3d_tex2d_;
    }

    bool OffscreenBufferWin::recreate() {
        onDestroy();

        if (width_ <= 0 || height_ <= 0) {
            DLOG(Log::ERR) << "Invalid size value.";
            return false;
        }

        return createHardwareBRT(width_, height_);
    }

    bool OffscreenBufferWin::createHardwareBRT(int width, int height) {
        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            width = int(std::ceil(width * img_options_.dpi_x / kDefaultDpi));
            height = int(std::ceil(height * img_options_.dpi_y / kDefaultDpi));
            break;
        case ImageDPIType::DEFAULT:
        default:
            break;
        }

        GPUDataFormat format;
        if (img_options_.pixel_format == ImagePixelFormat::HDR) {
            format = GPUDataFormat::R16G16B16A16_FLOAT;
        } else {
            format = GPUDataFormat::B8G8B8R8_UNORM;
        }

        d3d_tex2d_ = GPUTexture::createShaderTex2D(
            utl::num_cast<uint32_t>(width),
            utl::num_cast<uint32_t>(height), format, true);
        if (!d3d_tex2d_) {
            LOG(Log::WARNING) << "Failed to create offscreen rt!";
            return false;
        }

        auto tex2d_d3d = d3d_tex2d_.cast<GPUTexture2DD3D11>()->getNative();
        auto dxgi_surface = tex2d_d3d.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to query DXGI surface.";
            return false;
        }

        auto rt = DXMGR->createDXGIRenderTarget(
            dxgi_surface.get(), false, img_options_);
        if (!rt) {
            return false;
        }

        nrt_.setNative(rt);
        return true;
    }

}
}