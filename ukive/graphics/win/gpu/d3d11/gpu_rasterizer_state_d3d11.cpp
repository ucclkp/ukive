// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_rasterizer_state_d3d11.h"


namespace ukive {
namespace win {

    GPURasterizerStateD3D11::GPURasterizerStateD3D11(
        const utl::win::ComPtr<ID3D11RasterizerState>& rs)
        : rs_(rs) {}

    void GPURasterizerStateD3D11::setNative(const utl::win::ComPtr<ID3D11RasterizerState>& rs) {
        rs_ = rs;
    }

    ID3D11RasterizerState* GPURasterizerStateD3D11::getNative() const {
        return rs_.get();
    }

}
}