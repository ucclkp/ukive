// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_depth_stencil_d3d.h"


namespace ukive {
namespace win {

    GPUDepthStencilD3D::GPUDepthStencilD3D(
        const utl::win::ComPtr<ID3D11DepthStencilView>& dsv)
        : dsv_(dsv) {}

    void GPUDepthStencilD3D::setNative(const utl::win::ComPtr<ID3D11DepthStencilView>& dsv) {
        dsv_ = dsv;
    }

    ID3D11DepthStencilView* GPUDepthStencilD3D::getNative() const {
        return dsv_.get();
    }

}
}