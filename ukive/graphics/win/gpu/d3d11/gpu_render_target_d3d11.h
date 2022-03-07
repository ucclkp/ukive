// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_RENDER_TARGET_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_RENDER_TARGET_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_render_target.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPURenderTargetD3D11 :
        public GPURenderTarget,
        public GRefCountImpl
    {
    public:
        explicit GPURenderTargetD3D11(const utl::win::ComPtr<ID3D11RenderTargetView>& rtv);

        Native getOpaque() const override;

        void setNative(const utl::win::ComPtr<ID3D11RenderTargetView>& rtv);
        ID3D11RenderTargetView* getNative() const;

    private:
        utl::win::ComPtr<ID3D11RenderTargetView> rtv_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_RENDER_TARGET_D3D11_H_