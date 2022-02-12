// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_context_d3d.h"

#include <vector>

#include "ukive/graphics/win/gpu/gpu_buffer_d3d.h"
#include "ukive/graphics/win/gpu/gpu_d3d_utils.h"
#include "ukive/graphics/win/gpu/gpu_depth_stencil_d3d.h"
#include "ukive/graphics/win/gpu/gpu_depth_stencil_state_d3d.h"
#include "ukive/graphics/win/gpu/gpu_input_layout_d3d.h"
#include "ukive/graphics/win/gpu/gpu_rasterizer_state_d3d.h"
#include "ukive/graphics/win/gpu/gpu_render_target_d3d.h"
#include "ukive/graphics/win/gpu/gpu_sampler_state_d3d.h"
#include "ukive/graphics/win/gpu/gpu_shader_d3d.h"
#include "ukive/graphics/win/gpu/gpu_texture_d3d.h"
#include "ukive/graphics/win/gpu/gpu_shader_resource_d3d.h"


namespace ukive {
namespace win {

    GPUContextD3D::GPUContextD3D(const utl::win::ComPtr<ID3D11DeviceContext>& ctx)
        : d3d_context_(ctx) {}

    void GPUContextD3D::setVertexBuffers(
        uint32_t start_slot, uint32_t num, const GPUBuffer* buffers,
        const uint32_t* strides, const uint32_t* offset)
    {
        static std::vector<ID3D11Buffer*> arr;
        arr.clear();
        for (uint32_t i = 0; i < num; ++i) {
            arr.push_back(static_cast<const GPUBufferD3D*>(&buffers[i])->getNative());
        }

        d3d_context_->IASetVertexBuffers(start_slot, num, arr.data(), strides, offset);
    }

    void GPUContextD3D::setIndexBuffer(
        const GPUBuffer* buffer, GPUDataFormat format, uint32_t offset)
    {
        auto buf = static_cast<const GPUBufferD3D*>(buffer)->getNative();
        d3d_context_->IASetIndexBuffer(buf, mapFormat(format), offset);
    }

    void GPUContextD3D::setInputLayout(GPUInputLayout* layout) {
        auto native = static_cast<GPUInputLayoutD3D*>(layout)->getNative();
        d3d_context_->IASetInputLayout(native);
    }

    void GPUContextD3D::setPrimitiveTopology(Topology topology) {
        d3d_context_->IASetPrimitiveTopology(mapTopology(topology));
    }

    void GPUContextD3D::setRenderTargets(
        uint32_t num, const GPURenderTarget* rts, GPUDepthStencil* ds)
    {
        static std::vector<ID3D11RenderTargetView*> arr;
        arr.clear();
        for (uint32_t i = 0; i < num; ++i) {
            arr.push_back(static_cast<const GPURenderTargetD3D*>(&rts[i])->getNative());
        }

        ID3D11DepthStencilView* n_ds;
        if (ds) {
            n_ds = static_cast<GPUDepthStencilD3D*>(ds)->getNative();
        } else {
            n_ds = nullptr;
        }
        d3d_context_->OMSetRenderTargets(num, arr.data(), n_ds);
    }

    void GPUContextD3D::setDepthStencilState(
        GPUDepthStencilState* state, uint32_t stencil_ref)
    {
        auto n_state = static_cast<GPUDepthStencilStateD3D*>(state)->getNative();
        d3d_context_->OMSetDepthStencilState(n_state, stencil_ref);
    }

    void GPUContextD3D::setVertexShader(GPUShader* shader) {
        auto n_vs = static_cast<GPUVertexShaderD3D*>(shader)->getNative();
        d3d_context_->VSSetShader(n_vs, nullptr, 0);
    }

    void GPUContextD3D::setVConstantBuffers(
        uint32_t start_slot, uint32_t num, const GPUBuffer* buffers)
    {
        static std::vector<ID3D11Buffer*> arr;
        arr.clear();
        for (uint32_t i = 0; i < num; ++i) {
            arr.push_back(static_cast<const GPUBufferD3D*>(&buffers[i])->getNative());
        }

        d3d_context_->VSSetConstantBuffers(start_slot, num, arr.data());
    }

    void GPUContextD3D::setPixelShader(GPUShader* shader) {
        auto n_ps = static_cast<GPUPixelShaderD3D*>(shader)->getNative();
        d3d_context_->PSSetShader(n_ps, nullptr, 0);
    }

    void GPUContextD3D::setPSamplerStates(
        uint32_t start_slot, uint32_t num, const GPUSamplerState* states)
    {
        static std::vector<ID3D11SamplerState*> arr;
        arr.clear();
        for (uint32_t i = 0; i < num; ++i) {
            arr.push_back(static_cast<const GPUSamplerStateD3D*>(&states[i])->getNative());
        }

        d3d_context_->PSSetSamplers(start_slot, num, arr.data());
    }

    void GPUContextD3D::setPShaderResources(
        uint32_t start_slot, uint32_t num, const GPUShaderResource* res)
    {
        static std::vector<ID3D11ShaderResourceView*> arr;
        arr.clear();
        for (uint32_t i = 0; i < num; ++i) {
            arr.push_back(static_cast<const GPUShaderResourceD3D*>(&res[i])->getNative());
        }

        d3d_context_->PSSetShaderResources(start_slot, num, arr.data());
    }

    void GPUContextD3D::setViewport(uint32_t num, const Viewport* vps) {
        static std::vector<D3D11_VIEWPORT> arr;
        arr.clear();
        for (uint32_t i = 0; i < num; ++i) {
            D3D11_VIEWPORT n_vp;
            n_vp.Width = vps[i].width;
            n_vp.Height = vps[i].height;
            n_vp.TopLeftX = vps[i].x;
            n_vp.TopLeftY = vps[i].y;
            n_vp.MinDepth = vps[i].min_depth;
            n_vp.MaxDepth = vps[i].max_depth;
            arr.push_back(std::move(n_vp));
        }

        d3d_context_->RSSetViewports(num, arr.data());
    }

    void GPUContextD3D::setRasterizerState(GPURasterizerState* state) {
        auto n_state = static_cast<GPURasterizerStateD3D*>(state)->getNative();
        d3d_context_->RSSetState(n_state);
    }

    void GPUContextD3D::clearRenderTarget(GPURenderTarget* rt, const float rgba[4]) {
        auto n_rt = static_cast<GPURenderTargetD3D*>(rt)->getNative();
        d3d_context_->ClearRenderTargetView(n_rt, rgba);
    }

    void GPUContextD3D::clearDepthStencil(
        GPUDepthStencil* ds, uint32_t flags, float depth, uint8_t stencil)
    {
        auto n_ds = static_cast<GPUDepthStencilD3D*>(ds)->getNative();
        d3d_context_->ClearDepthStencilView(n_ds, flags, depth, stencil);
    }

    void GPUContextD3D::draw(uint32_t ver_count, uint32_t start_ver_loc) {
        d3d_context_->Draw(ver_count, start_ver_loc);
    }

    void GPUContextD3D::drawIndexed(
        uint32_t idx_count, uint32_t start_idx_loc, uint32_t base_ver_loc)
    {
        d3d_context_->DrawIndexed(idx_count, start_idx_loc, base_ver_loc);
    }

    void GPUContextD3D::drawInstanced(
        uint32_t ver_count_pre_inst, uint32_t inst_count,
        uint32_t start_ver_loc, uint32_t start_inst_loc)
    {
        d3d_context_->DrawInstanced(
            ver_count_pre_inst, inst_count, start_ver_loc, start_inst_loc);
    }

    void GPUContextD3D::drawIndexedInstanced(
        uint32_t idx_count_pre_inst, uint32_t inst_count,
        uint32_t start_idx_loc, int32_t base_ver_loc, uint32_t start_inst_loc)
    {
        d3d_context_->DrawIndexedInstanced(
            idx_count_pre_inst, inst_count, start_idx_loc, base_ver_loc, start_inst_loc);
    }

    void* GPUContextD3D::lock(GPUResource* resource) {
        ID3D11Resource* res = nullptr;

        switch (resource->getType()) {
        case GPUResource::Type::Buffer:
            res = static_cast<GPUBufferD3D*>(resource)->getNative();
            break;

        case GPUResource::Type::Texture:
        {
            auto tex = static_cast<GPUTexture*>(resource);
            auto& desc = tex->getDesc();
            switch (desc.dim) {
            case GPUTexture::Dimension::_1D:
                res = static_cast<GPUTexture1DD3D*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_2D:
                res = static_cast<GPUTexture2DD3D*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_3D:
                res = static_cast<GPUTexture3DD3D*>(tex)->getNative().get();
                break;
            }
            break;
        }
        }

        if (!res) {
            return nullptr;
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = d3d_context_->Map(res, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) {
            return nullptr;
        }

        return mappedResource.pData;
    }

    void GPUContextD3D::unlock(GPUResource* resource) {
        ID3D11Resource* res = nullptr;

        switch (resource->getType()) {
        case GPUResource::Type::Buffer:
            res = static_cast<GPUBufferD3D*>(resource)->getNative();
            break;

        case GPUResource::Type::Texture:
        {
            auto tex = static_cast<GPUTexture*>(resource);
            auto& desc = tex->getDesc();
            switch (desc.dim) {
            case GPUTexture::Dimension::_1D:
                res = static_cast<GPUTexture1DD3D*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_2D:
                res = static_cast<GPUTexture2DD3D*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_3D:
                break;
            }
            break;
        }
        }

        if (!res) {
            return;
        }

        d3d_context_->Unmap(res, 0);
    }

    utl::win::ComPtr<ID3D11DeviceContext> GPUContextD3D::getNative() const {
        return d3d_context_;
    }

}
}