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


namespace ukive {

    class LcImageFrameWin : public LcImageFrame {
    public:
        explicit LcImageFrameWin(const ComPtr<IWICBitmap>& source);

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float* dpi_x, float* dpi_y) const override;

        Size getSize() const override;

        bool getPixels(std::string* out, int* width, int* height) override;
        bool getBWPixels(std::string* out, int* width, int* height) override;

        ComPtr<IWICBitmap> getNative() const;

    private:
        ComPtr<IWICBitmap> native_bitmap_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FRAME_WIN_H_