// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_DEVICE_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_DEVICE_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_device.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUDeviceD3D11 :
        public GPUDevice,
        public GRefCountImpl
    {
    public:
        explicit GPUDeviceD3D11(const utl::win::ComPtr<ID3D11Device>& dev);

        GEcPtr<GPUBuffer> createBuffer(
            const GPUBuffer::Desc& desc, const GPUBuffer::ResourceData* data) override;
        GEcPtr<GPUInputLayout> createInputLayout(
            const GPUInputLayout::Desc* desc, size_t desc_count,
            const void* shader_bc, size_t size) override;
        GEcPtr<GPUTexture> createTexture(
            const GPUTexture::Desc& desc, const GPUBuffer::ResourceData* data) override;
        GEcPtr<GPURenderTarget> createRenderTarget(GPUResource* resource) override;
        GEcPtr<GPUDepthStencil> createDepthStencil(
            const GPUDepthStencil::Desc& desc, GPUResource* resource) override;
        GEcPtr<GPUShaderResource> createShaderResource(
            const GPUShaderResource::Desc* desc, GPUResource* resource) override;

        GEcPtr<GPUDepthStencilState> createDepthStencilState(const GPUDepthStencilState::Desc& desc) override;
        GEcPtr<GPURasterizerState> createRasterizerState(const GPURasterizerState::Desc& desc) override;
        GEcPtr<GPUSamplerState> createSamplerState(const GPUSamplerState::Desc& desc) override;

        GEcPtr<GPUShader> createVertexShader(const void* shader_bc, size_t size) override;
        GEcPtr<GPUShader> createPixelShader(const void* shader_bc, size_t size) override;

        GEcPtr<GPUTexture> openSharedTexture2D(intptr_t handle) override;

        utl::win::ComPtr<ID3D11Device> getNative() const;

    private:
        ID3D11Resource* convertResource(GPUResource* res) const;

        utl::win::ComPtr<ID3D11Device> d3d_device_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_DEVICE_D3D11_H_