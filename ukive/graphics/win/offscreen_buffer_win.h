// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_OFFSCREEN_BUFFER_H_
#define UKIVE_GRAPHICS_OFFSCREEN_BUFFER_H_

#include <memory>

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/cyro_render_target_d2d.h"
#include "ukive/graphics/win/directx_manager.h"


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

        ImageFrame* onExtractImage(const ImageOptions& options) override;

        Size getSize() const override;
        Size getPixelSize() const override;

        CyroRenderTarget* getRT() const override;
        const ImageOptions& getImageOptions() const override;

        utl::win::ComPtr<ID3D11Texture2D> getTexture() const;

    private:
        bool createHardwareBRT(int width, int height);

        int width_, height_;
        ImageOptions img_options_;
        utl::win::ComPtr<ID3D11Texture2D> d3d_tex2d_;
        std::unique_ptr<CyroRenderTargetD2D> rt_;
    };

}
}

#endif  // UKIVE_GRAPHICS_OFFSCREEN_BUFFER_H_