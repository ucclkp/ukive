// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_shader_resource_d3d11.h"


namespace ukive {
namespace win {

    GPUShaderResourceD3D11::GPUShaderResourceD3D11(
        const utl::win::ComPtr<ID3D11ShaderResourceView>& srv)
        : srv_(srv) {}

    void GPUShaderResourceD3D11::setNative(const utl::win::ComPtr<ID3D11ShaderResourceView>& srv) {
        srv_ = srv;
    }

    ID3D11ShaderResourceView* GPUShaderResourceD3D11::getNative() const {
        return srv_.get();
    }

}
}