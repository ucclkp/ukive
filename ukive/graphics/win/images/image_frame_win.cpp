// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_frame_win.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/app/application.h"
#include "ukive/graphics/win/directx_manager.h"
#include "ukive/graphics/win/images/image_options_d2d_utils.h"
#include "ukive/window/window_dpi_utils.h"


namespace ukive {
namespace win {

    ImageFrameWin::ImageFrameWin(
        const ImageOptions& options,
        const ImageRawParams& raw_params,
        const utl::win::ComPtr<IWICBitmapSource>& src,
        const utl::win::ComPtr<ID2D1Bitmap>& bmp)
        : ImageFrame(options),
          raw_params_(raw_params),
          wic_src_(src),
          native_bitmap_(bmp)
    {
        ubassert(bmp);
        initDpiValues();

        dev_guard_ = static_cast<DirectXManager*>(
            Application::getGraphicDeviceManager())->getGPUDeviceGuard();
    }

    ImageFrameWin::~ImageFrameWin() {}

    void ImageFrameWin::initDpiValues() {
        if (native_bitmap_) {
            native_bitmap_->GetDpi(&dpi_x_, &dpi_y_);
            return;
        }

        dpi_x_ = kDefaultDpi;
        dpi_y_ = kDefaultDpi;
    }

    void ImageFrameWin::setDpi(float dpi_x, float dpi_y) {
        if (dpi_x > 0 && dpi_y > 0) {
            dpi_x_ = dpi_x;
            dpi_y_ = dpi_y;
        }
    }

    void ImageFrameWin::getDpi(float* dpi_x, float* dpi_y) const {
        *dpi_x = dpi_x_;
        *dpi_y = dpi_y_;
    }

    SizeF ImageFrameWin::getSize() const {
        if (native_bitmap_) {
            auto size(native_bitmap_->GetSize());
            return SizeF(size.width, size.height);
        }
        return SizeF(0, 0);
    }

    SizeU ImageFrameWin::getPixelSize() const {
        if (native_bitmap_) {
            auto size(native_bitmap_->GetPixelSize());
            return SizeU(
                utl::num_cast<SizeU::type>(size.width),
                utl::num_cast<SizeU::type>(size.height));
        }
        return SizeU(0, 0);
    }

    bool ImageFrameWin::prepareForRender(ID2D1RenderTarget* rt) {
        if (!native_bitmap_) {
            return false;
        }

        if (dev_guard_.expired()) {
            native_bitmap_.reset();
            if (wic_src_) {
                D2D1_BITMAP_PROPERTIES props =
                    D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x_, dpi_y_);

                utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
                HRESULT hr = rt->CreateBitmapFromWicBitmap(wic_src_.get(), &props, &d2d_bmp);
                if (FAILED(hr)) {
                    return false;
                }
                native_bitmap_ = d2d_bmp;
            } else if (raw_params_.raw_data) {
                auto prop = mapBitmapProps(getOptions());

                utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
                HRESULT hr = rt->CreateBitmap(
                    D2D1::SizeU(raw_params_.width, raw_params_.height),
                    raw_params_.raw_data->getData(),
                    utl::num_cast<UINT32>(raw_params_.stride), prop, &d2d_bmp);
                if (FAILED(hr)) {
                    return false;
                }
                native_bitmap_ = d2d_bmp;
            } else {
                return false;
            }

            dev_guard_ = static_cast<DirectXManager*>(
                Application::getGraphicDeviceManager())->getGPUDeviceGuard();
            return true;
        }

        float dpi_x, dpi_y;
        native_bitmap_->GetDpi(&dpi_x, &dpi_y);
        if (!utl::is_num_equal(dpi_x, dpi_x_) || !utl::is_num_equal(dpi_y, dpi_y_)) {
            D2D1_BITMAP_PROPERTIES bmp_prop =
                D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x_, dpi_y_);

            utl::win::ComPtr<ID2D1Bitmap> new_bmp;
            HRESULT hr = rt->CreateSharedBitmap(
                IID_PPV_ARGS(&native_bitmap_), &bmp_prop, &new_bmp);
            if (FAILED(hr)) {
                return false;
            }
            native_bitmap_ = new_bmp;
        }

        return true;
    }

    utl::win::ComPtr<ID2D1Bitmap> ImageFrameWin::getNative() const {
        return native_bitmap_;
    }

}
}