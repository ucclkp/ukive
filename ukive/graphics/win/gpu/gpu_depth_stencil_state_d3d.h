// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_DEPTH_STENCIL_STATE_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_DEPTH_STENCIL_STATE_D3D_H_

#include "ukive/graphics/gpu/gpu_depth_stencil_state.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUDepthStencilStateD3D : public GPUDepthStencilState {
    public:
        explicit GPUDepthStencilStateD3D(const ComPtr<ID3D11DepthStencilState>& dss);

        void setNative(const ComPtr<ID3D11DepthStencilState>& dss);
        ID3D11DepthStencilState* getNative() const;

    private:
        ComPtr<ID3D11DepthStencilState> dss_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_DEPTH_STENCIL_STATE_D3D_H_