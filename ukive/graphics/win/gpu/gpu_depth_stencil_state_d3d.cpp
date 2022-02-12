// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_depth_stencil_state_d3d.h"


namespace ukive {
namespace win {

    GPUDepthStencilStateD3D::GPUDepthStencilStateD3D(
        const utl::win::ComPtr<ID3D11DepthStencilState>& dss)
        : dss_(dss) {}

    void GPUDepthStencilStateD3D::setNative(const utl::win::ComPtr<ID3D11DepthStencilState>& dss) {
        dss_ = dss;
    }

    ID3D11DepthStencilState* GPUDepthStencilStateD3D::getNative() const {
        return dss_.get();
    }

}
}