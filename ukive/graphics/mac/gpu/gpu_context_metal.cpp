// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_context_metal.h"


namespace ukive {

    GPUContextMetal::GPUContextMetal() {}

    void GPUContextMetal::setVertexBuffers(
        uint32_t start_slot, uint32_t num, const GPUBuffer* buffers,
        const uint32_t* strides, const uint32_t* offset)
    {

    }

    void GPUContextMetal::setIndexBuffer(
        const GPUBuffer* buffer, GPUDataFormat format, uint32_t offset)
    {

    }

    void GPUContextMetal::setInputLayout(GPUInputLayout* layout) {

    }

    void GPUContextMetal::setPrimitiveTopology(Topology topology) {

    }

    void GPUContextMetal::setRenderTargets(
        uint32_t num, const GPURenderTarget* rts, GPUDepthStencil* ds)
    {

    }

    void GPUContextMetal::setDepthStencilState(
        GPUDepthStencilState* state, uint32_t stencil_ref)
    {

    }


    void GPUContextMetal::setVertexShader(GPUShader* shader) {

    }

    void GPUContextMetal::setVConstantBuffers(
        uint32_t start_slot, uint32_t num, const GPUBuffer* buffers)
    {

    }

    void GPUContextMetal::setPixelShader(GPUShader* shader) {

    }

    void GPUContextMetal::setPSamplerStates(
        uint32_t start_slot, uint32_t num, const GPUSamplerState* states)
    {

    }

    void GPUContextMetal::setPShaderResources(
        uint32_t start_slot, uint32_t num, const GPUShaderResource* res)
    {

    }

    void GPUContextMetal::setViewport(uint32_t num, const Viewport* vps) {

    }

    void GPUContextMetal::setRasterizerState(GPURasterizerState* state) {

    }

    void GPUContextMetal::clearRenderTarget(GPURenderTarget* rt, const float rgba[4]) {

    }

    void GPUContextMetal::clearDepthStencil(
        GPUDepthStencil* ds, uint32_t flags, float depth, uint8_t stencil)
    {

    }

    void GPUContextMetal::draw(uint32_t ver_count, uint32_t start_ver_loc) {

    }

    void GPUContextMetal::drawIndexed(
        uint32_t idx_count, uint32_t start_idx_loc, uint32_t base_ver_loc)
    {

    }

    void GPUContextMetal::drawInstanced(
        uint32_t ver_count_pre_inst, uint32_t inst_count,
        uint32_t start_ver_loc, uint32_t start_inst_loc)
    {

    }

    void GPUContextMetal::drawIndexedInstanced(
        uint32_t idx_count_pre_inst, uint32_t inst_count,
        uint32_t start_idx_loc, int32_t base_ver_loc, uint32_t start_inst_loc)
    {

    }

}
