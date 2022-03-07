// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_shader_d3d11.h"


namespace ukive {
namespace win {

    // GPUVertexShaderD3D11
    GPUVertexShaderD3D11::GPUVertexShaderD3D11(
        const utl::win::ComPtr<ID3D11VertexShader>& vs)
        : vs_(vs) {}

    void GPUVertexShaderD3D11::setNative(const utl::win::ComPtr<ID3D11VertexShader>& vs) {
        vs_ = vs;
    }

    ID3D11VertexShader* GPUVertexShaderD3D11::getNative() const {
        return vs_.get();
    }


    // GPUPixelShaderD3D11
    GPUPixelShaderD3D11::GPUPixelShaderD3D11(
        const utl::win::ComPtr<ID3D11PixelShader>& ps)
        : ps_(ps) {}

    void GPUPixelShaderD3D11::setNative(const utl::win::ComPtr<ID3D11PixelShader>& ps) {
        ps_ = ps;
    }

    ID3D11PixelShader* GPUPixelShaderD3D11::getNative() const {
        return ps_.get();
    }

}
}