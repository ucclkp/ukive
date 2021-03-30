// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_shader_resource_d3d.h"


namespace ukive {

    GPUShaderResourceD3D::GPUShaderResourceD3D(
        const ComPtr<ID3D11ShaderResourceView>& srv)
        : srv_(srv) {}

    void GPUShaderResourceD3D::setNative(const ComPtr<ID3D11ShaderResourceView>& srv) {
        srv_ = srv;
    }

    ID3D11ShaderResourceView* GPUShaderResourceD3D::getNative() const {
        return srv_.get();
    }

}
