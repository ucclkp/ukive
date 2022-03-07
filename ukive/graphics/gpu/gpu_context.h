// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_CONTEXT_H_
#define UKIVE_GRAPHICS_GPU_GPU_CONTEXT_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gref_count.h"
#include "ukive/graphics/gpu/gpu_render_target.h"
#include "ukive/graphics/gpu/gpu_sampler_state.h"
#include "ukive/graphics/gpu/gpu_shader_resource.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUBuffer;
    class GPUShader;
    class GPUInputLayout;
    class GPURenderTarget;
    class GPUDepthStencil;
    class GPURasterizerState;
    class GPUDepthStencilState;
    class GPUSamplerState;
    class GPUShaderResource;
    class GPUResource;
    struct Viewport;

    class GPUContext : public virtual GRefCount {
    public:
        enum ClearFlags {
            CLEAR_DEPTH   = 1 << 0,
            CLEAR_STENCIL = 1 << 1,
        };

        enum class Topology {
            PointList,
            LineList,
            LineStrip,
            TriangleList,
            TriangleStrip,
        };

        virtual ~GPUContext() = default;

        // IA
        virtual void setVertexBuffers(
            uint32_t start_slot, uint32_t num, GPUBuffer* const* buffers,
            const uint32_t* strides, const uint32_t* offset) = 0;
        virtual void setIndexBuffer(
            const GPUBuffer* buffer, GPUDataFormat format, uint32_t offset) = 0;
        virtual void setInputLayout(GPUInputLayout* layout) = 0;
        virtual void setPrimitiveTopology(Topology topology) = 0;

        // OM
        virtual void setRenderTargets(
            uint32_t num, GPURenderTarget* const* rts, GPUDepthStencil* ds) = 0;
        virtual void setDepthStencilState(
            GPUDepthStencilState* state, uint32_t stencil_ref) = 0;

        // VS
        virtual void setVertexShader(GPUShader* shader) = 0;
        virtual void setVConstantBuffers(
            uint32_t start_slot, uint32_t num, GPUBuffer* const* buffers) = 0;

        // PS
        virtual void setPixelShader(GPUShader* shader) = 0;
        virtual void setPSamplerStates(
            uint32_t start_slot, uint32_t num, GPUSamplerState* const* states) = 0;
        virtual void setPShaderResources(
            uint32_t start_slot, uint32_t num, GPUShaderResource* const* res) = 0;

        // RS
        virtual void setViewports(uint32_t num, const Viewport* vps) = 0;
        virtual void setRasterizerState(GPURasterizerState* state) = 0;

        virtual void clearRenderTarget(GPURenderTarget* rt, const float rgba[4]) = 0;
        virtual void clearDepthStencil(
            GPUDepthStencil* ds, uint32_t flags, float depth, uint8_t stencil) = 0;

        virtual void draw(uint32_t ver_count, uint32_t start_ver_loc) = 0;
        virtual void drawIndexed(
            uint32_t idx_count, uint32_t start_idx_loc, uint32_t base_ver_loc) = 0;
        virtual void drawInstanced(
            uint32_t ver_count_pre_inst, uint32_t inst_count,
            uint32_t start_ver_loc, uint32_t start_inst_loc) = 0;
        virtual void drawIndexedInstanced(
            uint32_t idx_count_pre_inst, uint32_t inst_count,
            uint32_t start_idx_loc, int32_t base_ver_loc, uint32_t start_inst_loc) = 0;

        virtual void* lock(GPUResource* resource) = 0;
        virtual void unlock(GPUResource* resource) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_CONTEXT_H_