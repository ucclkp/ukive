// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_depth_stencil_d3d11.h"


namespace ukive {
namespace win {

    GPUDepthStencilD3D11::GPUDepthStencilD3D11(
        const utl::win::ComPtr<ID3D11DepthStencilView>& dsv)
        : dsv_(dsv) {}

    void GPUDepthStencilD3D11::setNative(const utl::win::ComPtr<ID3D11DepthStencilView>& dsv) {
        dsv_ = dsv;
    }

    ID3D11DepthStencilView* GPUDepthStencilD3D11::getNative() const {
        return dsv_.get();
    }

}
}