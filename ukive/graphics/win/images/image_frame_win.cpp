// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "image_frame_win.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/window/window_dpi_utils.h"


namespace ukive {
namespace win {

    ImageFrameWin::ImageFrameWin(
        const utl::win::ComPtr<ID2D1Bitmap>& bmp,
        const utl::win::ComPtr<ID2D1RenderTarget>& rt,
        const utl::win::ComPtr<IWICBitmapSource>& src)
        : wic_src_(src),
          native_bitmap_(bmp)
    {
        ubassert(wic_src_ && bmp);
        initDpiValues();
    }

    ImageFrameWin::ImageFrameWin(
        const utl::win::ComPtr<ID2D1Bitmap>& bmp,
        const utl::win::ComPtr<ID2D1RenderTarget>& rt,
        const GPtr<GPUContext>& ctx,
        const GPtr<GPUTexture>& src)
        : d3d_src_(src),
          native_bitmap_(bmp)
    {
        ubassert(d3d_src_ && bmp);
        initDpiValues();
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

            if (native_bitmap_ && compat_rt_) {
                D2D1_BITMAP_PROPERTIES bmp_prop =
                    D2D1::BitmapProperties(D2D1::PixelFormat(), dpi_x, dpi_y);

                utl::win::ComPtr<ID2D1Bitmap> new_bmp;
                HRESULT hr = compat_rt_->CreateSharedBitmap(
                    IID_PPV_ARGS(&native_bitmap_), &bmp_prop, &new_bmp);
                ubassert(SUCCEEDED(hr));
                if (SUCCEEDED(hr)) {
                    native_bitmap_ = new_bmp;
                }
            }
        }
    }

    void ImageFrameWin::getDpi(float* dpi_x, float* dpi_y) const {
        native_bitmap_->GetDpi(dpi_x, dpi_y);
    }

    SizeF ImageFrameWin::getSize() const {
        auto size(native_bitmap_->GetSize());
        return SizeF(size.width, size.height);
    }

    SizeU ImageFrameWin::getPixelSize() const {
        auto size(native_bitmap_->GetPixelSize());
        return SizeU(
            utl::num_cast<SizeU::type>(size.width),
            utl::num_cast<SizeU::type>(size.height));
    }

    bool ImageFrameWin::copyPixels(
        size_t stride, void* pixels, size_t buf_size)
    {
        return false;
    }

    void* ImageFrameWin::lockPixels() {
        return nullptr;
    }

    void ImageFrameWin::unlockPixels() {
    }

    utl::win::ComPtr<ID2D1Bitmap> ImageFrameWin::getNative() const {
        return native_bitmap_;
    }

}
}