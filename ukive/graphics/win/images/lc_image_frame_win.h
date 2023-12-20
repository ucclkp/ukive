// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_
#define UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gref_count_impl.h"
#include "ukive/graphics/images/lc_image_frame.h"

#include <wincodec.h>

#define ILF_TO_WIC_BMP(ilf)  static_cast<LcImageFrameWin*>(ilf)->getNative().get()
#define CILF_TO_WIC_BMP(ilf)  static_cast<const LcImageFrameWin*>(ilf)->getNative().get()
#define ILF_TO_WIC_BMP_SRC(ilf)  static_cast<LcImageFrameWin*>(ilf)->getNativeSrc().get()
#define CILF_TO_WIC_BMP_SRC(ilf)  static_cast<const LcImageFrameWin*>(ilf)->getNativeSrc().get()


namespace ukive {
namespace win {

    class LcImageFrameWin :
        public LcImageFrame,
        public GRefCountImpl
    {
    public:
        LcImageFrameWin(
            const ImageOptions& options,
            const GPtr<ByteData>& raw_data,
            const utl::win::ComPtr<IWICImagingFactory>& factory,
            const utl::win::ComPtr<IWICBitmapSource>& source);
        LcImageFrameWin(
            const ImageOptions& options,
            const GPtr<ByteData>& raw_data,
            const utl::win::ComPtr<IWICImagingFactory>& factory,
            const utl::win::ComPtr<IWICBitmap>& bitmap);

        bool createIfNecessary();

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float* dpi_x, float* dpi_y) const override;

        GPtr<LcImageFrame> scaleTo(const SizeU& frame_size) const override;
        GPtr<LcImageFrame> convertTo(const ImageOptions& options) const override;

        SizeF getSize() const override;
        SizeU getPixelSize() const override;

        bool copyPixels(
            size_t stride, void* pixels, size_t buf_size) override;
        void* lockPixels(unsigned int flags, size_t* stride) override;
        void unlockPixels() override;

        utl::win::ComPtr<IWICBitmap> getNative() const;
        utl::win::ComPtr<IWICBitmapSource> getNativeSrc() const;

    private:
        float dpi_x_ = 0;
        float dpi_y_ = 0;
        unsigned int lock_flags_ = 0;
        GPtr<ByteData> raw_data_;
        utl::win::ComPtr<IWICBitmap> native_bitmap_;
        utl::win::ComPtr<IWICBitmapLock> lock_;
        utl::win::ComPtr<IWICBitmapSource> native_src_;
        utl::win::ComPtr<IWICImagingFactory> wic_factory_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_