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

    class GPUDevice {
    public:
        virtual ~GPUDevice() = default;

        virtual GPUBuffer* createBuffer(
            const GPUBuffer::Desc* desc, const GPUBuffer::ResourceData* data) = 0;
        virtual GPUInputLayout* createInputLayout(
            const GPUInputLayout::Desc* desc, size_t desc_count,
            const void* shader_bc, size_t size) = 0;
        virtual GPUTexture* createTexture(
            const GPUTexture::Desc* desc, const GPUBuffer::ResourceData* data) = 0;
        virtual GPURenderTarget* createRenderTarget(GPUResource* resource) = 0;
        virtual GPUDepthStencil* createDepthStencil(
            const GPUDepthStencil::Desc* desc, GPUResource* resource) = 0;
        virtual GPUShaderResource* createShaderResource(
            const GPUShaderResource::Desc* desc, GPUResource* resource) = 0;

        virtual GPUDepthStencilState* createDepthStencilState(const GPUDepthStencilState::Desc* desc) = 0;
        virtual GPURasterizerState* createRasterizerState(const GPURasterizerState::Desc* desc) = 0;
        virtual GPUSamplerState* createSamplerState(const GPUSamplerState::Desc* desc) = 0;

        virtual GPUShader* createVertexShader(const void* shader_bc, size_t size) = 0;
        virtual GPUShader* createPixelShader(const void* shader_bc, size_t size) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_DEVICE_H_
