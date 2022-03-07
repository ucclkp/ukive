// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_device_metal.h"

#include "ukive/graphics/mac/gpu/metal/gpu_render_target_metal.h"
#include "ukive/graphics/mac/gpu/metal/gpu_shader_metal.h"


namespace ukive {
namespace mac {

    GPUDeviceMetal::GPUDeviceMetal() {}

    GEcPtr<GPUBuffer> GPUDeviceMetal::createBuffer(
        const GPUBuffer::Desc& desc,
        const GPUBuffer::ResourceData* data)
    {
        return {};
    }

    GEcPtr<GPUInputLayout> GPUDeviceMetal::createInputLayout(
        const GPUInputLayout::Desc* desc,
        size_t desc_count,
        const void* shader_bc,
        size_t size)
    {
        return {};
    }

    GEcPtr<GPUTexture> GPUDeviceMetal::createTexture(
        const GPUTexture::Desc& desc,
        const GPUBuffer::ResourceData* data)
    {
        return {};
    }

    GEcPtr<GPURenderTarget> GPUDeviceMetal::createRenderTarget(GPUResource* resource) {
        return {};
    }

    GEcPtr<GPUDepthStencil> GPUDeviceMetal::createDepthStencil(
        const GPUDepthStencil::Desc& desc,
        GPUResource* resource)
    {
        return {};
    }

    GEcPtr<GPUShaderResource> GPUDeviceMetal::createShaderResource(
        const GPUShaderResource::Desc* desc,
        GPUResource* resource)
    {
        return {};
    }

    GEcPtr<GPUDepthStencilState> GPUDeviceMetal::createDepthStencilState(const GPUDepthStencilState::Desc& desc) {
        return {};
    }

    GEcPtr<GPURasterizerState> GPUDeviceMetal::createRasterizerState(const GPURasterizerState::Desc& desc) {
        return {};
    }

    GEcPtr<GPUSamplerState> GPUDeviceMetal::createSamplerState(const GPUSamplerState::Desc& desc) {
        return {};
    }

    GEcPtr<GPUShader> GPUDeviceMetal::createVertexShader(const void* shader_bc, size_t size) {
        return {};
    }

    GEcPtr<GPUShader> GPUDeviceMetal::createPixelShader(const void* shader_bc, size_t size) {
        return {};
    }

    GEcPtr<GPUTexture> GPUDeviceMetal::openSharedTexture2D(intptr_t handle) {
        return {};
    }

}
}