// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/offscreen_buffer_win.h"

#include "utils/log.h"
#include "utils/number.hpp"

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
            size.width = int(std::ceil(width_ * img_options_.dpi_x / kDefaultDpi));
            size.height = int(std::ceil(height_ * img_options_.dpi_y / kDefaultDpi));
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

        d3d_tex2d_ = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createTexture2D(
                utl::num_cast<UINT>(width), utl::num_cast<UINT>(height),
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