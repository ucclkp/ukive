// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_DEVICE_METAL_H_
#define UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_DEVICE_METAL_H_

#include "ukive/graphics/gpu/gpu_device.h"
#include "ukive/graphics/gref_count_impl.h"


namespace ukive {
namespace mac {

    class GPUDeviceMetal :
        public GPUDevice,
        public GRefCountImpl
    {
    public:
        explicit GPUDeviceMetal();

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

    private:
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_DEVICE_METAL_H_