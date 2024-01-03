// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_context_d3d11.h"

#include <vector>

#include "utils/numbers.hpp"

#include "ukive/graphics/win/gpu/d3d11/gpu_buffer_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_d3d11_utils.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_depth_stencil_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_depth_stencil_state_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_input_layout_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_rasterizer_state_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_render_target_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_sampler_state_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_shader_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_texture_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_shader_resource_d3d11.h"

#define ARRAY_OR_VECTOR(Type, arr_num, real_num)  \
    Type* ptr;                  \
    Type arr[arr_num];          \
    std::vector<Type> vec;      \
    if (real_num > arr_num) {   \
        vec.resize(real_num);   \
        ptr = vec.data();       \
    } else { ptr = arr; }


namespace ukive {
namespace win {

    GPUContextD3D11::GPUContextD3D11(const utl::win::ComPtr<ID3D11DeviceContext>& ctx)
        : d3d_context_(ctx) {}

    void GPUContextD3D11::setVertexBuffers(
        uint32_t start_slot, uint32_t num, GPUBuffer* const* buffers,
        const uint32_t* strides, const uint32_t* offset)
    {
        ARRAY_OR_VECTOR(ID3D11Buffer*, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            auto nat = static_cast<const GPUBufferD3D11*>(buffers[i])->getNative();
            if (num > 8) {
                vec[i] = nat;
            } else {
                arr[i] = nat;
            }
        }

        d3d_context_->IASetVertexBuffers(start_slot, num, ptr, strides, offset);
    }

    void GPUContextD3D11::setIndexBuffer(
        const GPUBuffer* buffer, GPUDataFormat format, uint32_t offset)
    {
        auto buf = static_cast<const GPUBufferD3D11*>(buffer)->getNative();
        d3d_context_->IASetIndexBuffer(buf, mapFormat(format), offset);
    }

    void GPUContextD3D11::setInputLayout(GPUInputLayout* layout) {
        auto native = static_cast<GPUInputLayoutD3D11*>(layout)->getNative();
        d3d_context_->IASetInputLayout(native);
    }

    void GPUContextD3D11::setPrimitiveTopology(Topology topology) {
        d3d_context_->IASetPrimitiveTopology(mapTopology(topology));
    }

    void GPUContextD3D11::setRenderTargets(
        uint32_t num, GPURenderTarget* const* rts, GPUDepthStencil* ds)
    {
        ARRAY_OR_VECTOR(ID3D11RenderTargetView*, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            auto nat = static_cast<const GPURenderTargetD3D11*>(rts[i])->getNative();
            if (num > 8) {
                vec[i] = nat;
            } else {
                arr[i] = nat;
            }
        }

        ID3D11DepthStencilView* n_ds;
        if (ds) {
            n_ds = static_cast<GPUDepthStencilD3D11*>(ds)->getNative();
        } else {
            n_ds = nullptr;
        }
        d3d_context_->OMSetRenderTargets(num, ptr, n_ds);
    }

    void GPUContextD3D11::setDepthStencilState(
        GPUDepthStencilState* state, uint32_t stencil_ref)
    {
        auto n_state = static_cast<GPUDepthStencilStateD3D11*>(state)->getNative();
        d3d_context_->OMSetDepthStencilState(n_state, stencil_ref);
    }

    void GPUContextD3D11::setVertexShader(GPUShader* shader) {
        auto n_vs = static_cast<GPUVertexShaderD3D11*>(shader)->getNative();
        d3d_context_->VSSetShader(n_vs, nullptr, 0);
    }

    void GPUContextD3D11::setVConstantBuffers(
        uint32_t start_slot, uint32_t num, GPUBuffer* const* buffers)
    {
        ARRAY_OR_VECTOR(ID3D11Buffer*, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            auto nat = static_cast<const GPUBufferD3D11*>(buffers[i])->getNative();
            if (num > 8) {
                vec[i] = nat;
            } else {
                arr[i] = nat;
            }
        }

        d3d_context_->VSSetConstantBuffers(start_slot, num, ptr);
    }

    void GPUContextD3D11::setPixelShader(GPUShader* shader) {
        auto n_ps = static_cast<GPUPixelShaderD3D11*>(shader)->getNative();
        d3d_context_->PSSetShader(n_ps, nullptr, 0);
    }

    void GPUContextD3D11::setPSamplerStates(
        uint32_t start_slot, uint32_t num, GPUSamplerState* const* states)
    {
        ARRAY_OR_VECTOR(ID3D11SamplerState*, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            auto nat = static_cast<const GPUSamplerStateD3D11*>(states[i])->getNative();
            if (num > 8) {
                vec[i] = nat;
            } else {
                arr[i] = nat;
            }
        }

        d3d_context_->PSSetSamplers(start_slot, num, ptr);
    }

    void GPUContextD3D11::setPShaderResources(
        uint32_t start_slot, uint32_t num, GPUShaderResource* const* res)
    {
        ARRAY_OR_VECTOR(ID3D11ShaderResourceView*, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            auto nat = static_cast<const GPUShaderResourceD3D11*>(res[i])->getNative();
            if (num > 8) {
                vec[i] = nat;
            } else {
                arr[i] = nat;
            }
        }

        d3d_context_->PSSetShaderResources(start_slot, num, ptr);
    }

    void GPUContextD3D11::setPConstantBuffers(
        uint32_t start_slot, uint32_t num, GPUBuffer* const* buffers)
    {
        ARRAY_OR_VECTOR(ID3D11Buffer*, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            auto nat = static_cast<const GPUBufferD3D11*>(buffers[i])->getNative();
            if (num > 8) {
                vec[i] = nat;
            } else {
                arr[i] = nat;
            }
        }

        d3d_context_->PSSetConstantBuffers(start_slot, num, ptr);
    }

    void GPUContextD3D11::setViewports(uint32_t num, const Viewport* vps) {
        ARRAY_OR_VECTOR(D3D11_VIEWPORT, 8, num);
        for (uint32_t i = 0; i < num; ++i) {
            D3D11_VIEWPORT n_vp;
            n_vp.Width = vps[i].width;
            n_vp.Height = vps[i].height;
            n_vp.TopLeftX = vps[i].x;
            n_vp.TopLeftY = vps[i].y;
            n_vp.MinDepth = vps[i].min_depth;
            n_vp.MaxDepth = vps[i].max_depth;

            if (num > 8) {
                vec[i] = n_vp;
            } else {
                arr[i] = n_vp;
            }
        }

        d3d_context_->RSSetViewports(num, ptr);
    }

    void GPUContextD3D11::setRasterizerState(GPURasterizerState* state) {
        auto n_state = static_cast<GPURasterizerStateD3D11*>(state)->getNative();
        d3d_context_->RSSetState(n_state);
    }

    void GPUContextD3D11::clearRenderTarget(GPURenderTarget* rt, const float rgba[4]) {
        auto n_rt = static_cast<GPURenderTargetD3D11*>(rt)->getNative();
        d3d_context_->ClearRenderTargetView(n_rt, rgba);
    }

    void GPUContextD3D11::clearDepthStencil(
        GPUDepthStencil* ds, uint32_t flags, float depth, uint8_t stencil)
    {
        auto n_ds = static_cast<GPUDepthStencilD3D11*>(ds)->getNative();
        d3d_context_->ClearDepthStencilView(n_ds, flags, depth, stencil);
    }

    void GPUContextD3D11::draw(uint32_t ver_count, uint32_t start_ver_loc) {
        d3d_context_->Draw(ver_count, start_ver_loc);
    }

    void GPUContextD3D11::drawIndexed(
        uint32_t idx_count, uint32_t start_idx_loc, uint32_t base_ver_loc)
    {
        d3d_context_->DrawIndexed(idx_count, start_idx_loc, base_ver_loc);
    }

    void GPUContextD3D11::drawInstanced(
        uint32_t ver_count_pre_inst, uint32_t inst_count,
        uint32_t start_ver_loc, uint32_t start_inst_loc)
    {
        d3d_context_->DrawInstanced(
            ver_count_pre_inst, inst_count, start_ver_loc, start_inst_loc);
    }

    void GPUContextD3D11::drawIndexedInstanced(
        uint32_t idx_count_pre_inst, uint32_t inst_count,
        uint32_t start_idx_loc, int32_t base_ver_loc, uint32_t start_inst_loc)
    {
        d3d_context_->DrawIndexedInstanced(
            idx_count_pre_inst, inst_count, start_idx_loc, base_ver_loc, start_inst_loc);
    }

    void* GPUContextD3D11::lock(
        GPUResource* resource,
        unsigned int type, size_t* row_stride)
    {
        ID3D11Resource* res = nullptr;

        switch (resource->getType()) {
        case GPUResource::Type::Buffer:
            res = static_cast<GPUBufferD3D11*>(resource)->getNative();
            break;

        case GPUResource::Type::Texture:
        {
            auto tex = static_cast<GPUTexture*>(resource);
            auto& desc = tex->getDesc();
            switch (desc.dim) {
            case GPUTexture::Dimension::_1D:
                res = static_cast<GPUTexture1DD3D11*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_2D:
                res = static_cast<GPUTexture2DD3D11*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_3D:
                res = static_cast<GPUTexture3DD3D11*>(tex)->getNative().get();
                break;
            }
            break;
        }
        }

        if (!res) {
            return nullptr;
        }

        D3D11_MAP map_type;
        switch (type) {
        case LOCK_READ:  map_type = D3D11_MAP_READ; break;
        case LOCK_WRITE: map_type = D3D11_MAP_WRITE_DISCARD; break;
        default:         map_type = D3D11_MAP_WRITE_DISCARD; break;
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = d3d_context_->Map(res, 0, map_type, 0, &mappedResource);
        if (FAILED(hr)) {
            return nullptr;
        }

        if (row_stride) {
            *row_stride = utl::num_cast<size_t>(mappedResource.RowPitch);
        }

        return mappedResource.pData;
    }

    void GPUContextD3D11::unlock(GPUResource* resource) {
        ID3D11Resource* res = nullptr;

        switch (resource->getType()) {
        case GPUResource::Type::Buffer:
            res = static_cast<GPUBufferD3D11*>(resource)->getNative();
            break;

        case GPUResource::Type::Texture:
        {
            auto tex = static_cast<GPUTexture*>(resource);
            auto& desc = tex->getDesc();
            switch (desc.dim) {
            case GPUTexture::Dimension::_1D:
                res = static_cast<GPUTexture1DD3D11*>(tex)->getNative().get();
                break;
            case GPUTexture::Dimension::_2D:
                res = static_cast<GPUTexture2DD3D11*>(tex)->getNative().get();
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

    utl::win::ComPtr<ID3D11DeviceContext> GPUContextD3D11::getNative() const {
        return d3d_context_;
    }

}
}