// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_SHADER_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_SHADER_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUVertexShaderD3D11 :
        public GPUShader,
        public GRefCountImpl
    {
    public:
        explicit GPUVertexShaderD3D11(const utl::win::ComPtr<ID3D11VertexShader>& vs);

        void setNative(const utl::win::ComPtr<ID3D11VertexShader>& vs);
        ID3D11VertexShader* getNative() const;

    private:
        utl::win::ComPtr<ID3D11VertexShader> vs_;
    };


    class GPUPixelShaderD3D11 :
        public GPUShader,
        public GRefCountImpl
    {
    public:
        explicit GPUPixelShaderD3D11(const utl::win::ComPtr<ID3D11PixelShader>& ps);

        void setNative(const utl::win::ComPtr<ID3D11PixelShader>& ps);
        ID3D11PixelShader* getNative() const;

    private:
        utl::win::ComPtr<ID3D11PixelShader> ps_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_SHADER_D3D11_H_