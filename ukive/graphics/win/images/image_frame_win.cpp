// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/images/image_frame_win.h"

#include "utils/numbers.hpp"


namespace ukive {
namespace win {

    ImageFrameWin::ImageFrameWin(
        const utl::win::ComPtr<ID2D1Bitmap>& bmp,
        const utl::win::ComPtr<ID2D1RenderTarget>& rt,
        const utl::win::ComPtr<IWICBitmapSource>& src)
        : wic_src_(src),
          native_bitmap_(bmp)
    {
    }

    ImageFrameWin::ImageFrameWin(
        const utl::win::ComPtr<ID2D1Bitmap>& bmp,
        const utl::win::ComPtr<ID2D1RenderTarget>& rt,
        const utl::win::ComPtr<ID3D11DeviceContext>& ctx,
        const utl::win::ComPtr<ID3D11Texture2D>& src)
        : d3d_src_(src),
          native_bitmap_(bmp)
    {
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

    utl::win::ComPtr<ID2D1Bitmap> ImageFrameWin::getNative() const {
        return native_bitmap_;
    }

}
}