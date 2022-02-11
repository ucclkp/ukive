// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_SHADER_RESOURCE_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_SHADER_RESOURCE_D3D_H_

#include "ukive/graphics/gpu/gpu_shader_resource.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUShaderResourceD3D : public GPUShaderResource {
    public:
        explicit GPUShaderResourceD3D(
            const ComPtr<ID3D11ShaderResourceView>& srv);

        void setNative(const ComPtr<ID3D11ShaderResourceView>& srv);
        ID3D11ShaderResourceView* getNative() const;

    private:
        ComPtr<ID3D11ShaderResourceView> srv_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_SHADER_RESOURCE_D3D_H_