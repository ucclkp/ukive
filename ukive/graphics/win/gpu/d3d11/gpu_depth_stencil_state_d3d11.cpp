// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_depth_stencil_state_d3d11.h"


namespace ukive {
namespace win {

    GPUDepthStencilStateD3D11::GPUDepthStencilStateD3D11(
        const utl::win::ComPtr<ID3D11DepthStencilState>& dss)
        : dss_(dss) {}

    void GPUDepthStencilStateD3D11::setNative(const utl::win::ComPtr<ID3D11DepthStencilState>& dss) {
        dss_ = dss;
    }

    ID3D11DepthStencilState* GPUDepthStencilStateD3D11::getNative() const {
        return dss_.get();
    }

}
}