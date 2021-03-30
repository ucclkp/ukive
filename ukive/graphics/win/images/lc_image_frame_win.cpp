// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/images/lc_image_frame_win.h"

#include "utils/number.hpp"
#include "utils/log.h"


namespace ukive {

    LcImageFrameWin::LcImageFrameWin(const ComPtr<IWICBitmap>& source)
        : native_bitmap_(source) {
    }

    void LcImageFrameWin::setDpi(float dpi_x, float dpi_y) {
        if (dpi_x > 0 && dpi_y > 0) {
            native_bitmap_->SetResolution(dpi_x, dpi_y);
        }
    }

    void LcImageFrameWin::getDpi(float* dpi_x, float* dpi_y) const {
        double dx, dy;
        native_bitmap_->GetResolution(&dx, &dy);
        *dpi_x = float(dx);
        *dpi_y = float(dy);
    }

    Size LcImageFrameWin::getSize() const {
        UINT width = 0, height = 0;
        HRESULT hr = native_bitmap_->GetSize(&width, &height);
        if (FAILED(hr)) {
            DCHECK(false);
        }
        return Size(
            utl::num_cast<Size::type>(width),
            utl::num_cast<Size::type>(height));
    }

    bool LcImageFrameWin::getPixels(
        std::string* out, int* width, int* height)
    {
        auto size = getSize();
        *width = size.width;
        *height = size.height;

        out->resize((*width) * (*height) * 4);
        HRESULT hr = native_bitmap_->CopyPixels(
            nullptr,
            (*width) * 4, (*width) * (*height) * 4,
            reinterpret_cast<BYTE*>(&*out->begin()));
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool LcImageFrameWin::getBWPixels(
        std::string* out, int* width, int* height)
    {
        auto size = getSize();
        *width = size.width;
        *height = size.height;

        UINT stride = ((*width) + 7) / 8;
        out->resize(stride * (*height));
        HRESULT hr = native_bitmap_->CopyPixels(
            nullptr, stride, stride * (*height),
            reinterpret_cast<BYTE*>(&*out->begin()));
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    ComPtr<IWICBitmap> LcImageFrameWin::getNative() const {
        return native_bitmap_;
    }

}
