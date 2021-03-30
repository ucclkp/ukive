// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_RENDER_TARGET_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_RENDER_TARGET_D3D_H_

#include "ukive/graphics/gpu/gpu_render_target.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {

    class GPURenderTargetD3D : public GPURenderTarget {
    public:
        explicit GPURenderTargetD3D(const ComPtr<ID3D11RenderTargetView>& rtv);

        Native getOpaque() const override;

        void setNative(const ComPtr<ID3D11RenderTargetView>& rtv);
        ID3D11RenderTargetView* getNative() const;

    private:
        ComPtr<ID3D11RenderTargetView> rtv_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_RENDER_TARGET_D3D_H_