// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_DEVICE_H_
#define UKIVE_GRAPHICS_GPU_GPU_DEVICE_H_

#include <cstddef>

#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gpu/gpu_depth_stencil.h"
#include "ukive/graphics/gpu/gpu_depth_stencil_state.h"
#include "ukive/graphics/gpu/gpu_rasterizer_state.h"
#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gpu/gpu_sampler_state.h"
#include "ukive/graphics/gpu/gpu_input_layout.h"
#include "ukive/graphics/gpu/gpu_texture.h"
#include "ukive/graphics/gpu/gpu_shader_resource.h"


namespace ukive {

    class GPUBuffer;
    class GPUShader;
    class GPUInputLayout;
    class GPURenderTarget;
    class GPUDepthStencilState;
    class GPURasterizerState;
    class GPUSamplerState;
    class GPUShaderResource;

    class GPUDevice : public virtual GRefCount {
    public:
        virtual ~GPUDevice() = default;

        virtual GEcPtr<GPUBuffer> createBuffer(
            const GPUBuffer::Desc& desc, const GPUBuffer::ResourceData* data) = 0;
        virtual GEcPtr<GPUInputLayout> createInputLayout(
            const GPUInputLayout::Desc* desc, size_t desc_count,
            const void* shader_bc, size_t size) = 0;
        virtual GEcPtr<GPUTexture> createTexture(
            const GPUTexture::Desc& desc, const GPUBuffer::ResourceData* data) = 0;
        virtual GEcPtr<GPURenderTarget> createRenderTarget(GPUResource* resource) = 0;
        virtual GEcPtr<GPUDepthStencil> createDepthStencil(
            const GPUDepthStencil::Desc& desc, GPUResource* resource) = 0;
        virtual GEcPtr<GPUShaderResource> createShaderResource(
            const GPUShaderResource::Desc* desc, GPUResource* resource) = 0;

        virtual GEcPtr<GPUDepthStencilState> createDepthStencilState(const GPUDepthStencilState::Desc& desc) = 0;
        virtual GEcPtr<GPURasterizerState> createRasterizerState(const GPURasterizerState::Desc& desc) = 0;
        virtual GEcPtr<GPUSamplerState> createSamplerState(const GPUSamplerState::Desc& desc) = 0;

        virtual GEcPtr<GPUShader> createVertexShader(const void* shader_bc, size_t size) = 0;
        virtual GEcPtr<GPUShader> createPixelShader(const void* shader_bc, size_t size) = 0;

        virtual GEcPtr<GPUTexture> openSharedTexture2D(intptr_t handle) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_DEVICE_H_
