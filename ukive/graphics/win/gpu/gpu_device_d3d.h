// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_DEVICE_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_DEVICE_D3D_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_device.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUDeviceD3D : public GPUDevice {
    public:
        explicit GPUDeviceD3D(const utl::win::ComPtr<ID3D11Device>& dev);

        GPUBuffer* createBuffer(
            const GPUBuffer::Desc* desc, const GPUBuffer::ResourceData* data) override;
        GPUInputLayout* createInputLayout(
            const GPUInputLayout::Desc* desc, size_t desc_count,
            const void* shader_bc, size_t size) override;
        GPUTexture* createTexture(
            const GPUTexture::Desc* desc, const GPUBuffer::ResourceData* data) override;
        GPURenderTarget* createRenderTarget(GPUResource* resource) override;
        GPUDepthStencil* createDepthStencil(
            const GPUDepthStencil::Desc* desc, GPUResource* resource) override;
        GPUShaderResource* createShaderResource(
            const GPUShaderResource::Desc* desc, GPUResource* resource) override;

        GPUDepthStencilState* createDepthStencilState(const GPUDepthStencilState::Desc* desc) override;
        GPURasterizerState* createRasterizerState(const GPURasterizerState::Desc* desc) override;
        GPUSamplerState* createSamplerState(const GPUSamplerState::Desc* desc) override;

        GPUShader* createVertexShader(const void* shader_bc, size_t size) override;
        GPUShader* createPixelShader(const void* shader_bc, size_t size) override;

        utl::win::ComPtr<ID3D11Device> getNative() const;

    private:
        ID3D11Resource* convertResource(GPUResource* res) const;

        utl::win::ComPtr<ID3D11Device> d3d_device_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_DEVICE_D3D_H_