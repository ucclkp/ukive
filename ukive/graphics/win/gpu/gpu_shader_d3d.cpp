// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_shader_d3d.h"


namespace ukive {
namespace win {

    // GPUVertexShaderD3D
    GPUVertexShaderD3D::GPUVertexShaderD3D(
        const ComPtr<ID3D11VertexShader>& vs)
        : vs_(vs) {}

    void GPUVertexShaderD3D::setNative(const ComPtr<ID3D11VertexShader>& vs) {
        vs_ = vs;
    }

    ID3D11VertexShader* GPUVertexShaderD3D::getNative() const {
        return vs_.get();
    }


    // GPUPixelShaderD3D
    GPUPixelShaderD3D::GPUPixelShaderD3D(
        const ComPtr<ID3D11PixelShader>& ps)
        : ps_(ps) {}

    void GPUPixelShaderD3D::setNative(const ComPtr<ID3D11PixelShader>& ps) {
        ps_ = ps;
    }

    ID3D11PixelShader* GPUPixelShaderD3D::getNative() const {
        return ps_.get();
    }

}
}