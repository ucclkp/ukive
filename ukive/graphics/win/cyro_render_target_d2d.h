// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_CYRO_RENDER_TARGET_D2D_H_
#define UKIVE_GRAPHICS_WIN_CYRO_RENDER_TARGET_D2D_H_

#include "ukive/graphics/cyro_render_target.h"
#include "ukive/graphics/win/directx_manager.h"


namespace ukive {
namespace win {

    class CyroRenderTargetD2D : public CyroRenderTarget {
    public:
        CyroRenderTargetD2D() = default;

        GPtr<ImageFrame> createSharedImageFrame(
            const GPtr<GPUTexture>& texture, const ImageOptions& options) override;
        void destroy() override;

        void setNative(const utl::win::ComPtr<ID2D1RenderTarget>& rt);
        utl::win::ComPtr<ID2D1RenderTarget> getNative() const;

    private:
        utl::win::ComPtr<ID2D1RenderTarget> rt_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_CYRO_RENDER_TARGET_D2D_H_