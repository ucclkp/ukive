// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGE_IMAGE_FRAME_WIN_H_
#define UKIVE_GRAPHICS_WIN_IMAGE_IMAGE_FRAME_WIN_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/byte_data.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count_impl.h"
#include "ukive/graphics/images/image_frame.h"
#include "ukive/graphics/rebuildable.h"

#include <d2d1.h>
#include <wincodec.h>

#define IF_TO_D2D_BMP(frame)  static_cast<ImageFrameWin*>(frame)->getNative().get()
#define CIF_TO_D2D_BMP(frame)  static_cast<const ImageFrameWin*>(frame)->getNative().get()


namespace ukive {
namespace win {

    class ImageFrameWin :
        public ImageFrame,
        public GRefCountImpl,
        public Rebuildable
    {
    public:
        struct ImageRawParams {
            int width;
            int height;
            GPtr<ByteData> raw_data;
            size_t stride;
        };

        ImageFrameWin(
            const ImageOptions& options,
            const ImageRawParams& raw_params,
            const utl::win::ComPtr<IWICBitmapSource>& src,
            const utl::win::ComPtr<ID2D1Bitmap>& bmp);
        ~ImageFrameWin();

        void setDpi(float dpi_x, float dpi_y) override;
        void getDpi(float* dpi_x, float* dpi_y) const override;

        SizeF getSize() const override;
        SizeU getPixelSize() const override;

        bool prepareForRender(ID2D1RenderTarget* rt);

        utl::win::ComPtr<ID2D1Bitmap> getNative() const;

    protected:
        void onDemolish() override;
        void onRebuild(bool succeeded) override;

    private:
        void initDpiValues();
        bool recreate(ID2D1RenderTarget* rt, ID2D1Bitmap** out);

        float dpi_x_ = 0;
        float dpi_y_ = 0;
        ImageRawParams raw_params_;
        utl::win::ComPtr<IWICBitmapSource> wic_src_;
        utl::win::ComPtr<ID2D1Bitmap> d2d_bmp_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_IMAGE_FRAME_WIN_H_