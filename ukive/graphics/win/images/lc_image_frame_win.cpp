// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/images/lc_image_frame_win.h"

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/window/window_dpi_utils.h"


namespace ukive {
namespace win {

    LcImageFrameWin::LcImageFrameWin(
        const ImageOptions& options,
        const GPtr<ByteData>& raw_data,
        const utl::win::ComPtr<IWICImagingFactory>& factory,
        const utl::win::ComPtr<IWICBitmapSource>& source)
        : LcImageFrame(options),
          raw_data_(raw_data),
          native_src_(source),
          wic_factory_(factory)
    {
        ubassert(source);
        ubassert(factory);
        LcImageFrameWin::getDpi(&dpi_x_, &dpi_y_);
    }

    LcImageFrameWin::LcImageFrameWin(
        const ImageOptions& options,
        const GPtr<ByteData>& raw_data,
        const utl::win::ComPtr<IWICImagingFactory>& factory,
        const utl::win::ComPtr<IWICBitmap>& bitmap)
        : LcImageFrame(options),
          raw_data_(raw_data),
          native_bitmap_(bitmap),
          native_src_(bitmap.cast<IWICBitmapSource>()),
          wic_factory_(factory)
    {
        ubassert(bitmap);
        ubassert(factory);
        LcImageFrameWin::getDpi(&dpi_x_, &dpi_y_);
    }

    bool LcImageFrameWin::createIfNecessary() {
        if (native_bitmap_) {
            return true;
        }

        HRESULT hr = wic_factory_->CreateBitmapFromSource(
            native_src_.get(), WICBitmapCacheOnDemand, &native_bitmap_);
        if (FAILED(hr)) {
            ubassert(false);
            return false;
        }

        hr = native_bitmap_->SetResolution(dpi_x_, dpi_y_);
        ubassert(SUCCEEDED(hr));

        return true;
    }

    void LcImageFrameWin::setDpi(float dpi_x, float dpi_y) {
        if (dpi_x > 0 && dpi_y > 0) {
            dpi_x_ = dpi_x;
            dpi_y_ = dpi_y;

            if (native_bitmap_) {
                HRESULT hr = native_bitmap_->SetResolution(dpi_x, dpi_y);
                ubassert(SUCCEEDED(hr));
            }
        }
    }

    void LcImageFrameWin::getDpi(float* dpi_x, float* dpi_y) const {
        if (dpi_x) *dpi_x = dpi_x_;
        if (dpi_y) *dpi_y = dpi_y_;
    }

    SizeF LcImageFrameWin::getSize() const {
        float dpi_x, dpi_y;
        getDpi(&dpi_x, &dpi_y);
        auto size = getPixelSize();
        return SizeF(
            size.width() / (dpi_x / kDefaultDpi),
            size.height() / (dpi_y / kDefaultDpi));
    }

    SizeU LcImageFrameWin::getPixelSize() const {
        UINT width = 0, height = 0;
        HRESULT hr = native_src_->GetSize(&width, &height);
        ubassert(SUCCEEDED(hr));
        return SizeU(
            utl::num_cast<SizeU::type>(width),
            utl::num_cast<SizeU::type>(height));
    }

    bool LcImageFrameWin::copyPixels(
        size_t stride, void* pixels, size_t buf_size)
    {
        HRESULT hr = native_src_->CopyPixels(
            nullptr,
            utl::num_cast<UINT>(stride),
            utl::num_cast<UINT>(buf_size), static_cast<BYTE*>(pixels));
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    void* LcImageFrameWin::lockPixels(unsigned int flags, size_t* stride) {
        if (!createIfNecessary()) {
            return nullptr;
        }

        if (lock_) {
            if (lock_flags_ != flags) {
                lock_.reset();
                lock_flags_ = 0;
            }
        }

        HRESULT hr;
        if (!lock_) {
            DWORD a_flags = 0;
            if (flags & IAF_READ) {
                a_flags |= WICBitmapLockRead;
            }
            if (flags & IAF_WRITE) {
                a_flags |= WICBitmapLockWrite;
            }

            hr = native_bitmap_->Lock(nullptr, a_flags, &lock_);
            if (FAILED(hr)) {
                return nullptr;
            }
        }

        UINT buf_size;
        WICInProcPointer ptr;
        hr = lock_->GetDataPointer(&buf_size, &ptr);
        if (FAILED(hr)) {
            lock_.reset();
            return nullptr;
        }

        UINT row_stride;
        hr = lock_->GetStride(&row_stride);
        if (FAILED(hr)) {
            lock_.reset();
            return nullptr;
        }

        lock_flags_ = flags;
        *stride = utl::num_cast<size_t>(row_stride);
        return ptr;
    }

    void LcImageFrameWin::unlockPixels() {
        lock_.reset();
    }

    utl::win::ComPtr<IWICBitmap> LcImageFrameWin::getNative() const {
        return native_bitmap_;
    }

    utl::win::ComPtr<IWICBitmapSource> LcImageFrameWin::getNativeSrc() const {
        return native_src_;
    }

}
}