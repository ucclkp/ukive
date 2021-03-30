// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGE_IMAGE_FRAME_WIN_H_
#define UKIVE_GRAPHICS_WIN_IMAGE_IMAGE_FRAME_WIN_H_

#include "ukive/graphics/images/image_frame.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d2d1.h>

#define IF_TO_D2D_BMP(frame)  static_cast<ImageFrameWin*>(frame)->getNative().get()
#define CIF_TO_D2D_BMP(frame)  static_cast<const ImageFrameWin*>(frame)->getNative().get()


namespace ukive {

    class ImageFrameWin : public ImageFrame {
    public:
        explicit ImageFrameWin(const ComPtr<ID2D1Bitmap>& source);

        void getDpi(float* dpi_x, float* dpi_y) const override;

        SizeF getSize() const override;
        Size getPixelSize() const override;

        ComPtr<ID2D1Bitmap> getNative() const;

    private:
        ComPtr<ID2D1Bitmap> native_bitmap_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_IMAGE_FRAME_WIN_H_