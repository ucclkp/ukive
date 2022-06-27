// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_frame_win.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

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
          d2d_bmp_(bmp)
    {
        ubassert(bmp);
        initDpiValues();
    }

    ImageFrameWin::~ImageFrameWin() {}

    void ImageFrameWin::onDemolish() {
        d2d_bmp_.reset();
    }

    void ImageFrameWin::onRebuild(bool succeeded) {}

    void ImageFrameWin::initDpiValues() {
        if (d2d_bmp_) {
            d2d_bmp_->GetDpi(&dpi_x_, &dpi_y_);
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
        if (d2d_bmp_) {
            auto size(d2d_bmp_->GetSize());
            return SizeF(size.width, size.height);
        }
        return SizeF(0, 0);
    }

    SizeU ImageFrameWin::getPixelSize() const {
        if (d2d_bmp_) {
            auto size(d2d_bmp_->GetPixelSize());
            return SizeU(
                utl::num_cast<SizeU::type>(size.width),
                utl::num_cast<SizeU::type>(size.height));
        }
        return SizeU(0, 0);
    }

    bool ImageFrameWin::prepareForRender(ID2D1RenderTarget* rt) {
        if (!d2d_bmp_) {
            if (wic_src_) {
                D2D1_BITMAP_PROPERTIES props =
                    D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x_, dpi_y_);

                utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
                HRESULT hr = rt->CreateBitmapFromWicBitmap(wic_src_.get(), &props, &d2d_bmp);
                if (FAILED(hr)) {
                    return false;
                }
                d2d_bmp_ = d2d_bmp;
            } else if (raw_params_.raw_data) {
                auto prop = mapBitmapProps(getOptions());

                utl::win::ComPtr<ID2D1Bitmap> d2d_bmp;
                HRESULT hr = rt->CreateBitmap(
                    D2D1::SizeU(raw_params_.width, raw_params_.height),
                    raw_params_.raw_data->getConstData(),
                    utl::num_cast<UINT32>(raw_params_.stride), prop, &d2d_bmp);
                if (FAILED(hr)) {
                    return false;
                }
                d2d_bmp_ = d2d_bmp;
            } else {
                return false;
            }
            return true;
        }

        float dpi_x, dpi_y;
        d2d_bmp_->GetDpi(&dpi_x, &dpi_y);
        if (!utl::is_num_equal(dpi_x, dpi_x_) || !utl::is_num_equal(dpi_y, dpi_y_)) {
            D2D1_BITMAP_PROPERTIES bmp_prop =
                D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x_, dpi_y_);

            utl::win::ComPtr<ID2D1Bitmap> new_bmp;
            HRESULT hr = rt->CreateSharedBitmap(
                IID_PPV_ARGS(&d2d_bmp_), &bmp_prop, &new_bmp);
            if (FAILED(hr)) {
                return false;
            }
            d2d_bmp_ = new_bmp;
        }

        return true;
    }

    utl::win::ComPtr<ID2D1Bitmap> ImageFrameWin::getNative() const {
        return d2d_bmp_;
    }

}
}