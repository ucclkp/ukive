// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_rasterizer_state_d3d.h"


namespace ukive {

    GPURasterizerStateD3D::GPURasterizerStateD3D(
        const ComPtr<ID3D11RasterizerState>& rs)
        : rs_(rs) {}

    void GPURasterizerStateD3D::setNative(const ComPtr<ID3D11RasterizerState>& rs) {
        rs_ = rs;
    }

    ID3D11RasterizerState* GPURasterizerStateD3D::getNative() const {
        return rs_.get();
    }

}
