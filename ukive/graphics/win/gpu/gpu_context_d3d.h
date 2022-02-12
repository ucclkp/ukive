// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_CONTEXT_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_CONTEXT_D3D_H_

#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUContextD3D : public GPUContext {
    public:
        explicit GPUContextD3D(const ComPtr<ID3D11DeviceContext>& ctx);

        void setVertexBuffers(
            uint32_t start_slot, uint32_t num, const GPUBuffer* buffers,
            const uint32_t* strides, const uint32_t* offset) override;
        void setIndexBuffer(
            const GPUBuffer* buffer, GPUDataFormat format, uint32_t offset) override;

        void setInputLayout(GPUInputLayout* layout) override;
        void setPrimitiveTopology(Topology topology) override;

        void setRenderTargets(
            uint32_t num, const GPURenderTarget* rts, GPUDepthStencil* ds) override;
        void setDepthStencilState(
            GPUDepthStencilState* state, uint32_t stencil_ref) override;

        void setVertexShader(GPUShader* shader) override;
        void setVConstantBuffers(
            uint32_t start_slot, uint32_t num, const GPUBuffer* buffers) override;

        void setPixelShader(GPUShader* shader) override;
        void setPSamplerStates(
            uint32_t start_slot, uint32_t num, const GPUSamplerState* states) override;
        void setPShaderResources(
            uint32_t start_slot, uint32_t num, const GPUShaderResource* res) override;

        void setViewport(uint32_t num, const Viewport* vps) override;
        void setRasterizerState(GPURasterizerState* state) override;

        void clearRenderTarget(GPURenderTarget* rt, const float rgba[4]) override;
        void clearDepthStencil(
            GPUDepthStencil* ds, uint32_t flags, float depth, uint8_t stencil) override;

        void draw(uint32_t ver_count, uint32_t start_ver_loc) override;
        void drawIndexed(
            uint32_t idx_count, uint32_t start_idx_loc, uint32_t base_ver_loc) override;
        void drawInstanced(
            uint32_t ver_count_pre_inst, uint32_t inst_count,
            uint32_t start_ver_loc, uint32_t start_inst_loc) override;
        void drawIndexedInstanced(
            uint32_t idx_count_pre_inst, uint32_t inst_count,
            uint32_t start_idx_loc, int32_t base_ver_loc, uint32_t start_inst_loc) override;

        void* lock(GPUResource* resource) override;
        void unlock(GPUResource* resource) override;

        ComPtr<ID3D11DeviceContext> getNative() const;

    private:
        ComPtr<ID3D11DeviceContext> d3d_context_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_CONTEXT_D3D_H_