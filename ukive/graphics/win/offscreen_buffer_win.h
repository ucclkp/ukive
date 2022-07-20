// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_OFFSCREEN_BUFFER_WIN_H_
#define UKIVE_GRAPHICS_WIN_OFFSCREEN_BUFFER_WIN_H_

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/gpu/gpu_texture.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/native_rt_d2d.h"


namespace ukive {
namespace win {

    class OffscreenBufferWin : public OffscreenBuffer
    {
    public:
        OffscreenBufferWin();

        bool onCreate(
            int width, int height,
            const ImageOptions& options) override;
        GRet onResize(int width, int height) override;
        void onDPIChange(float dpi_x, float dpi_y) override;
        void onDestroy() override;

        void onBeginDraw() override;
        GRet onEndDraw() override;

        GPtr<ImageFrame> onExtractImage(const ImageOptions& options) override;

        Size getSize() const override;
        Size getPixelSize() const override;

        const NativeRT* getNativeRT() const override;
        const ImageOptions& getImageOptions() const override;

        GPtr<GPUTexture> getTexture() const;

    private:
        bool recreate();
        bool createHardwareBRT(int width, int height);

        int width_, height_;
        ImageOptions img_options_;
        GPtr<GPUTexture> d3d_tex2d_;
        NativeRTD2D nrt_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_OFFSCREEN_BUFFER_WIN_H_