// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/offscreen_buffer_win.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/graphics/win/images/image_frame_win.h"
#include "ukive/window/window_dpi_utils.h"


namespace ukive {

    OffscreenBufferWin::OffscreenBufferWin()
        : width_(0), height_(0),
          rt_(std::make_unique<CyroRenderTargetD2D>()) {
    }

    bool OffscreenBufferWin::onCreate(
        size_t width, size_t height,
        const ImageOptions& options)
    {
        width_ = width;
        height_ = height;
        img_options_ = options;
        return createHardwareBRT(width_, height_);
    }

    GRet OffscreenBufferWin::onResize(size_t width, size_t height) {
        width_ = width;
        height_ = height;
        return createHardwareBRT(width, height) ? GRet::Succeeded : GRet::Failed;
    }

    void OffscreenBufferWin::onDPIChange(float dpi_x, float dpi_y) {
        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            img_options_.dpi_x = dpi_x;
            img_options_.dpi_y = dpi_y;
            rt_->getNative()->SetDpi(dpi_x, dpi_y);
            break;
        default:
            break;
        }
    }

    void OffscreenBufferWin::onDestroy() {
        rt_->destroy();
        d3d_tex2d_.reset();
    }

    void OffscreenBufferWin::onBeginDraw() {
        rt_->getNative()->BeginDraw();
    }

    GRet OffscreenBufferWin::onEndDraw() {
        HRESULT hr = rt_->getNative()->EndDraw();
        if (FAILED(hr)) {
            if (hr == D2DERR_RECREATE_TARGET) {
                return GRet::Retry;
            }
            LOG(Log::ERR) << "Failed to draw d2d content.";
            return GRet::Failed;
        }
        return GRet::Succeeded;
    }

    ImageFrame* OffscreenBufferWin::onExtractImage(const ImageOptions& options) {
        D2D1_BITMAP_PROPERTIES bmp_prop = mapBitmapProps(options);

        ComPtr<ID2D1Bitmap> bitmap;
        HRESULT hr = rt_->getNative()->CreateSharedBitmap(
            __uuidof(IDXGISurface), d3d_tex2d_.cast<IDXGISurface>().get(), &bmp_prop, &bitmap);
        if (FAILED(hr)) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED ||
                hr == DXGI_ERROR_DEVICE_RESET)
            {
                return {};
            }
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return nullptr;
        }

        return new ImageFrameWin(bitmap);
    }

    Size OffscreenBufferWin::getSize() const {
        return Size(width_, height_);
    }

    Size OffscreenBufferWin::getPixelSize() const {
        Size size;
        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            size.width = size_t(std::ceil(width_ * img_options_.dpi_x / kDefaultDpi));
            size.height = size_t(std::ceil(height_ * img_options_.dpi_y / kDefaultDpi));
            break;
        case ImageDPIType::DEFAULT:
        default:
            size.width = width_;
            size.height = height_;
            break;
        }
        return size;
    }

    CyroRenderTarget* OffscreenBufferWin::getRT() const {
        return rt_.get();
    }

    const ImageOptions& OffscreenBufferWin::getImageOptions() const {
        return img_options_;
    }

    ComPtr<ID3D11Texture2D> OffscreenBufferWin::getTexture() const {
        return d3d_tex2d_;
    }

    bool OffscreenBufferWin::createHardwareBRT(size_t width, size_t height) {
        DCHECK(width <= std::numeric_limits<UINT>::max());
        DCHECK(height <= std::numeric_limits<UINT>::max());

        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            width = size_t(std::ceil(width * img_options_.dpi_x / kDefaultDpi));
            height = size_t(std::ceil(height * img_options_.dpi_y / kDefaultDpi));
            break;
        case ImageDPIType::DEFAULT:
        default:
            break;
        }

        d3d_tex2d_ = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createTexture2D(
                UINT(width), UINT(height),
                true, img_options_.pixel_format == ImagePixelFormat::HDR, false);
        if (!d3d_tex2d_) {
            return false;
        }

        auto dxgi_surface = d3d_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to query DXGI surface.";
            return false;
        }

        auto rt = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createDXGIRenderTarget(dxgi_surface.get(), false, img_options_);
        if (!rt) {
            return false;
        }

        rt_->setNative(rt);
        return true;
    }

}