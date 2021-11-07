// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_
#define UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_

#include "ukive/graphics/images/lc_image_frame.h"
#include "ukive/system/win/com_ptr.hpp"

#include <wincodec.h>

#define ILF_TO_WIC_BMP(ilf)  static_cast<LcImageFrameWin*>(ilf)->getNative().get()
#define CILF_TO_WIC_BMP(ilf)  static_cast<const LcImageFrameWin*>(ilf)->getNative().get()
#define ILF_TO_WIC_BMP_SRC(ilf)  static_cast<LcImageFrameWin*>(ilf)->getNativeSrc().get()
#define CILF_TO_WIC_BMP_SRC(ilf)  static_cast<const LcImageFrameWin*>(ilf)->getNativeSrc().get()


namespace ukive {

    class LcImageFrameWin : public LcImageFrame {
    public:
        LcImageFrameWin(
            const ComPtr<IWICImagingFactory>& factory,
            const ComPtr<IWICBitmapSource>& source);
        LcImageFrameWin(
            const ComPtr<IWICImagingFactory>& factory,
            const ComPtr<IWICBitmap>& bitmap);

        bool createIfNecessary();

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float* dpi_x, float* dpi_y) const override;

        SizeF getSize() const override;
        SizeU getPixelSize() const override;

        bool copyPixels(
            size_t stride, uint8_t* pixels, size_t buf_size) override;
        uint8_t* lockPixels() override;
        void unlockPixels() override;

        ComPtr<IWICBitmap> getNative() const;
        ComPtr<IWICBitmapSource> getNativeSrc() const;

    private:
        float dpi_x_ = 0;
        float dpi_y_ = 0;
        ComPtr<IWICBitmap> native_bitmap_;
        ComPtr<IWICBitmapLock> lock_;
        ComPtr<IWICBitmapSource> native_src_;
        ComPtr<IWICImagingFactory> wic_factory_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_