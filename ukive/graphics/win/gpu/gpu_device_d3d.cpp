// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_device_d3d.h"

#include <vector>

#include "utils/log.h"
#include "utils/number.hpp"

#include "ukive/graphics/win/gpu/gpu_buffer_d3d.h"
#include "ukive/graphics/win/gpu/gpu_input_layout_d3d.h"
#include "ukive/graphics/win/gpu/gpu_d3d_utils.h"
#include "ukive/graphics/win/gpu/gpu_texture_d3d.h"
#include "ukive/graphics/win/gpu/gpu_render_target_d3d.h"
#include "ukive/graphics/win/gpu/gpu_depth_stencil_d3d.h"
#include "ukive/graphics/win/gpu/gpu_depth_stencil_state_d3d.h"
#include "ukive/graphics/win/gpu/gpu_rasterizer_state_d3d.h"
#include "ukive/graphics/win/gpu/gpu_sampler_state_d3d.h"
#include "ukive/graphics/win/gpu/gpu_shader_d3d.h"
#include "ukive/graphics/win/gpu/gpu_shader_resource_d3d.h"


namespace ukive {

    GPUDeviceD3D::GPUDeviceD3D(const ComPtr<ID3D11Device>& dev)
        : d3d_device_(dev) {}

    GPUBuffer* GPUDeviceD3D::createBuffer(
        const GPUBuffer::Desc* desc, const GPUBuffer::ResourceData* data)
    {
        D3D11_BUFFER_DESC vb_desc;
        vb_desc.BindFlags = mapBindType(desc->res_type);
        vb_desc.CPUAccessFlags = mapCPUAccessFlags(desc->cpu_access_flag);
        vb_desc.Usage = desc->is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        vb_desc.ByteWidth = desc->byte_width;
        vb_desc.MiscFlags = 0;
        vb_desc.StructureByteStride = desc->struct_byte_stride;

        HRESULT hr;
        ComPtr<ID3D11Buffer> d3d_buffer;
        if (data) {
            D3D11_SUBRESOURCE_DATA vertex_data;
            vertex_data.pSysMem = data->sys_mem;
            vertex_data.SysMemPitch = data->slice_pitch;
            vertex_data.SysMemSlicePitch = data->slice_pitch;

            hr = d3d_device_->CreateBuffer(&vb_desc, &vertex_data, &d3d_buffer);
        } else {
            hr = d3d_device_->CreateBuffer(&vb_desc, nullptr, &d3d_buffer);
        }

        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUBufferD3D(d3d_buffer, *desc);
    }

    GPUInputLayout* GPUDeviceD3D::createInputLayout(
        const GPUInputLayout::Desc* desc, size_t desc_count,
        const void* shader_bc, size_t size)
    {
        static std::vector<D3D11_INPUT_ELEMENT_DESC> layouts;
        layouts.clear();
        for (size_t i = 0; i < desc_count; ++i) {
            auto& item = desc[i];

            D3D11_INPUT_ELEMENT_DESC layout;
            layout.SemanticName = item.name;
            layout.SemanticIndex = item.index;
            layout.Format = mapFormat(item.format);
            layout.InputSlot = item.input_slot;
            if (item.aligned_byte_offset == GPUInputLayout::Append) {
                layout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            } else {
                layout.AlignedByteOffset = item.aligned_byte_offset;
            }
            layout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layout.InstanceDataStepRate = 0;

            layouts.push_back(std::move(layout));
        }

        ComPtr<ID3D11InputLayout> d3d_layout;
        HRESULT hr = d3d_device_->CreateInputLayout(
            layouts.data(), utl::num_cast<UINT>(desc_count), shader_bc, size, &d3d_layout);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUInputLayoutD3D(d3d_layout);
    }

    GPUTexture* GPUDeviceD3D::createTexture(
        const GPUTexture::Desc* desc, const GPUBuffer::ResourceData* data)
    {
        GPUTexture* result = nullptr;
        D3D11_SUBRESOURCE_DATA res_data;
        if (data) {
            res_data.pSysMem = data->sys_mem;
            res_data.SysMemPitch = data->pitch;
            res_data.SysMemSlicePitch = data->slice_pitch;
        }

        switch (desc->dim) {
        case GPUTexture::Dimension::_1D:
        {
            D3D11_TEXTURE1D_DESC tex_desc;
            tex_desc.ArraySize = 1;
            tex_desc.BindFlags = mapBindType(desc->res_type);
            tex_desc.Format = mapFormat(desc->format);
            tex_desc.Width = desc->width;
            tex_desc.MipLevels = desc->mip_levels;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.Usage = desc->is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
            tex_desc.MiscFlags = 0;

            ComPtr<ID3D11Texture1D> t;
            HRESULT hr = d3d_device_->CreateTexture1D(&tex_desc, data ? &res_data : nullptr, &t);
            if (FAILED(hr)) {
                return nullptr;
            }
            result = new GPUTexture1DD3D(t, *desc);
            break;
        }

        case GPUTexture::Dimension::_2D:
        {
            D3D11_TEXTURE2D_DESC tex_desc;
            tex_desc.ArraySize = 1;
            tex_desc.BindFlags = mapBindType(desc->res_type);
            tex_desc.Format = mapFormat(desc->format);
            tex_desc.Width = desc->width;
            tex_desc.Height = desc->height;
            tex_desc.MipLevels = desc->mip_levels;
            tex_desc.SampleDesc.Count = 1;
            tex_desc.SampleDesc.Quality = 0;
            tex_desc.MiscFlags = 0;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.Usage = desc->is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

            ComPtr<ID3D11Texture2D> t;
            HRESULT hr = d3d_device_->CreateTexture2D(&tex_desc, data ? &res_data : nullptr, &t);
            if (FAILED(hr)) {
                return nullptr;
            }
            result = new GPUTexture2DD3D(t, *desc);
            break;
        }

        case GPUTexture::Dimension::_3D:
        {
            D3D11_TEXTURE3D_DESC tex_desc;
            tex_desc.BindFlags = mapBindType(desc->res_type);
            tex_desc.Format = mapFormat(desc->format);
            tex_desc.Width = desc->width;
            tex_desc.Height = desc->height;
            tex_desc.Depth = desc->depth;
            tex_desc.MipLevels = desc->mip_levels;
            tex_desc.MiscFlags = 0;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.Usage = desc->is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

            ComPtr<ID3D11Texture3D> t;
            HRESULT hr = d3d_device_->CreateTexture3D(&tex_desc, data ? &res_data : nullptr, &t);
            if (FAILED(hr)) {
                return nullptr;
            }
            result = new GPUTexture3DD3D(t, *desc);
            break;
        }
        }
        return result;
    }

    GPURenderTarget* GPUDeviceD3D::createRenderTarget(GPUResource* resource) {
        ID3D11Resource* d3d_res = convertResource(resource);
        if (!d3d_res) {
            return nullptr;
        }

        ComPtr<ID3D11RenderTargetView> d3d_rt;
        HRESULT hr = d3d_device_->CreateRenderTargetView(d3d_res, nullptr, &d3d_rt);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPURenderTargetD3D(d3d_rt);
    }

    GPUDepthStencil* GPUDeviceD3D::createDepthStencil(
        const GPUDepthStencil::Desc* desc, GPUResource* resource)
    {
        ID3D11Resource* d3d_res = convertResource(resource);
        if (!d3d_res) {
            return nullptr;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
        dsv_desc.Format = mapFormat(desc->format);
        dsv_desc.ViewDimension = mapDSVDeminsion(desc->view_dim);
        dsv_desc.Flags = desc->flags;

        switch (desc->view_dim) {
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE1D:
            dsv_desc.Texture1D.MipSlice = desc->tex1d.mip_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE1DARRAY:
            dsv_desc.Texture1DArray.MipSlice = desc->tex1d_array.mip_slice;
            dsv_desc.Texture1DArray.ArraySize = desc->tex1d_array.array_size;
            dsv_desc.Texture1DArray.FirstArraySlice = desc->tex1d_array.first_array_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2D:
            dsv_desc.Texture2D.MipSlice = desc->tex2d.mip_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DARRAY:
            dsv_desc.Texture2DArray.MipSlice = desc->tex2d_array.mip_slice;
            dsv_desc.Texture2DArray.ArraySize = desc->tex2d_array.array_size;
            dsv_desc.Texture2DArray.FirstArraySlice = desc->tex2d_array.first_array_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DMS:
            dsv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DMSARRAY:
            dsv_desc.Texture2DMSArray.ArraySize = desc->tex2dms_array.array_size;
            dsv_desc.Texture2DMSArray.FirstArraySlice = desc->tex2dms_array.first_array_slice;
            break;
        default:
            return nullptr;
        }

        ComPtr<ID3D11DepthStencilView> d3d_dsv;
        HRESULT hr = d3d_device_->CreateDepthStencilView(d3d_res, &dsv_desc, &d3d_dsv);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUDepthStencilD3D(d3d_dsv);
    }

    GPUShaderResource* GPUDeviceD3D::createShaderResource(
        const GPUShaderResource::Desc* desc, GPUResource* resource)
    {
        ID3D11Resource* d3d_res = convertResource(resource);
        if (!d3d_res) {
            return nullptr;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        srv_desc.Format = mapFormat(desc->format);
        srv_desc.ViewDimension = mapSRVDeminsion(desc->view_dim);

        switch (desc->view_dim) {
        case GPUShaderResource::SRV_DIMENSION_BUFFER:
            srv_desc.Buffer.ElementOffset = desc->buffer.element_offset;
            srv_desc.Buffer.FirstElement = desc->buffer.first_element;
            srv_desc.Buffer.ElementWidth = desc->buffer.element_width;
            srv_desc.Buffer.NumElements = desc->buffer.element_num;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE1D:
            srv_desc.Texture1D.MipLevels = desc->tex1d.mip_levels;
            srv_desc.Texture1D.MostDetailedMip = desc->tex1d.most_detailed_mip;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE1DARRAY:
            srv_desc.Texture1DArray.MipLevels = desc->tex1d_array.mip_levels;
            srv_desc.Texture1DArray.MostDetailedMip = desc->tex1d_array.most_detailed_mip;
            srv_desc.Texture1DArray.ArraySize = desc->tex1d_array.array_size;
            srv_desc.Texture1DArray.FirstArraySlice = desc->tex1d_array.first_array_slice;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2D:
            srv_desc.Texture2D.MipLevels = desc->tex2d.mip_levels;
            srv_desc.Texture2D.MostDetailedMip = desc->tex2d.most_detailed_mip;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2DARRAY:
            srv_desc.Texture2DArray.MipLevels = desc->tex2d_array.mip_levels;
            srv_desc.Texture2DArray.MostDetailedMip = desc->tex2d_array.most_detailed_mip;
            srv_desc.Texture2DArray.ArraySize = desc->tex2d_array.array_size;
            srv_desc.Texture2DArray.FirstArraySlice = desc->tex2d_array.first_array_slice;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2DMS:
            srv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2DMSARRAY:
            srv_desc.Texture2DMSArray.FirstArraySlice = desc->tex2dms_array.first_array_slice;
            srv_desc.Texture2DMSArray.ArraySize = desc->tex2dms_array.array_size;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE3D:
            srv_desc.Texture3D.MipLevels = desc->tex3d.mip_levels;
            srv_desc.Texture3D.MostDetailedMip = desc->tex3d.most_detailed_mip;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURECUBE:
            srv_desc.TextureCube.MipLevels = desc->tex_cube.mip_levels;
            srv_desc.TextureCube.MostDetailedMip = desc->tex_cube.most_detailed_mip;
            break;
        case GPUShaderResource::SRV_DIMENSION_TEXTURECUBEARRAY:
            srv_desc.TextureCubeArray.MipLevels = desc->tex_cube_array.mip_levels;
            srv_desc.TextureCubeArray.MostDetailedMip = desc->tex_cube_array.most_detailed_mip;
            srv_desc.TextureCubeArray.First2DArrayFace = desc->tex_cube_array.first_2d_array_face;
            srv_desc.TextureCubeArray.NumCubes = desc->tex_cube_array.cube_num;
            break;
        case GPUShaderResource::SRV_DIMENSION_BUFFEREX:
            srv_desc.BufferEx.FirstElement = desc->buffer_ex.first_element;
            srv_desc.BufferEx.NumElements = desc->buffer_ex.element_num;
            srv_desc.BufferEx.Flags = desc->buffer_ex.flags;
            break;
        default:
            return nullptr;
        }

        ComPtr<ID3D11ShaderResourceView> d3d_srv;
        HRESULT hr = d3d_device_->CreateShaderResourceView(d3d_res, &srv_desc, &d3d_srv);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUShaderResourceD3D(d3d_srv);
    }

    GPUDepthStencilState* GPUDeviceD3D::createDepthStencilState(const GPUDepthStencilState::Desc* desc) {
        D3D11_DEPTH_STENCIL_DESC dds_desc;
        dds_desc.DepthEnable = desc->depth_enabled ? TRUE : FALSE;
        dds_desc.DepthWriteMask = mapWriteMask(desc->depth_write_mask);
        dds_desc.DepthFunc = mapComparisonFunc(desc->depth_func);
        dds_desc.StencilEnable = desc->stencil_enabled ? TRUE : FALSE;
        dds_desc.StencilReadMask = desc->stencil_read_mask;
        dds_desc.StencilWriteMask = desc->stencil_write_mask;

        dds_desc.FrontFace.StencilFailOp = mapStencilOp(desc->front_face.stencil_fail_op);
        dds_desc.FrontFace.StencilDepthFailOp = mapStencilOp(desc->front_face.stencil_depth_fail_op);
        dds_desc.FrontFace.StencilPassOp = mapStencilOp(desc->front_face.stencil_pass_op);
        dds_desc.FrontFace.StencilFunc = mapComparisonFunc(desc->front_face.stencil_func);

        dds_desc.BackFace.StencilFailOp = mapStencilOp(desc->back_face.stencil_fail_op);
        dds_desc.BackFace.StencilDepthFailOp = mapStencilOp(desc->back_face.stencil_depth_fail_op);
        dds_desc.BackFace.StencilPassOp = mapStencilOp(desc->back_face.stencil_pass_op);
        dds_desc.BackFace.StencilFunc = mapComparisonFunc(desc->back_face.stencil_func);

        ComPtr<ID3D11DepthStencilState> dds_state;
        HRESULT hr = d3d_device_->CreateDepthStencilState(&dds_desc, &dds_state);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUDepthStencilStateD3D(dds_state);
    }

    GPURasterizerState* GPUDeviceD3D::createRasterizerState(const GPURasterizerState::Desc* desc) {
        D3D11_RASTERIZER_DESC r_desc;
        r_desc.FillMode = mapFillMode(desc->fill_mode);
        r_desc.CullMode = mapCullMode(desc->cull_mode);
        r_desc.FrontCounterClockwise = desc->front_counter_clockwise ? TRUE : FALSE;
        r_desc.DepthBias = 0;
        r_desc.DepthBiasClamp = 0;
        r_desc.SlopeScaledDepthBias = 0;
        r_desc.DepthClipEnable = desc->depth_clip_enabled ? TRUE : FALSE;
        r_desc.ScissorEnable = desc->scissor_enabled ? TRUE : FALSE;
        r_desc.MultisampleEnable = desc->multisample_enabled ? TRUE : FALSE;
        r_desc.AntialiasedLineEnable = desc->antialiased_line_enabled ? TRUE : FALSE;

        ComPtr<ID3D11RasterizerState> rs;
        HRESULT hr = d3d_device_->CreateRasterizerState(&r_desc, &rs);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPURasterizerStateD3D(rs);
    }

    GPUSamplerState* GPUDeviceD3D::createSamplerState(const GPUSamplerState::Desc* desc) {
        D3D11_SAMPLER_DESC s_desc;
        s_desc.Filter = mapFilter(desc->filter);
        s_desc.AddressU = mapAddrMode(desc->addr_u);
        s_desc.AddressV = mapAddrMode(desc->addr_v);
        s_desc.AddressW = mapAddrMode(desc->addr_w);
        s_desc.MipLODBias = desc->mip_lod_bias;
        s_desc.MaxAnisotropy = desc->max_anisotropy;
        s_desc.ComparisonFunc = mapComparisonFunc(desc->comp_func);
        std::memcpy(s_desc.BorderColor, desc->border_color, sizeof(float) * 4);
        s_desc.MinLOD = desc->min_lod;
        s_desc.MaxLOD = desc->max_lod;

        ComPtr<ID3D11SamplerState> ss;
        HRESULT hr = d3d_device_->CreateSamplerState(&s_desc, &ss);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUSamplerStateD3D(ss);
    }

    GPUShader* GPUDeviceD3D::createVertexShader(const void* shader_bc, size_t size) {
        ComPtr<ID3D11VertexShader> vs;
        HRESULT hr = d3d_device_->CreateVertexShader(shader_bc, size, nullptr, &vs);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUVertexShaderD3D(vs);
    }

    GPUShader* GPUDeviceD3D::createPixelShader(const void* shader_bc, size_t size) {
        ComPtr<ID3D11PixelShader> ps;
        HRESULT hr = d3d_device_->CreatePixelShader(shader_bc, size, nullptr, &ps);
        if (FAILED(hr)) {
            return nullptr;
        }

        return new GPUPixelShaderD3D(ps);
    }

    ID3D11Resource* GPUDeviceD3D::convertResource(GPUResource* res) const {
        ID3D11Resource* d3d_res = nullptr;
        switch (res->getType()) {
        case GPUResource::Type::Buffer:
            d3d_res = static_cast<GPUBufferD3D*>(res)->getNative();
            break;
        case GPUResource::Type::Texture:
        {
            auto& tex_desc = static_cast<GPUTexture*>(res)->getDesc();
            switch (tex_desc.dim) {
            case GPUTexture::Dimension::_1D:
                d3d_res = static_cast<GPUTexture1DD3D*>(res)->getNative();
                break;
            case GPUTexture::Dimension::_2D:
                d3d_res = static_cast<GPUTexture2DD3D*>(res)->getNative();
                break;
            case GPUTexture::Dimension::_3D:
                d3d_res = static_cast<GPUTexture3DD3D*>(res)->getNative();
                break;
            }
            break;
        }
        }
        return d3d_res;
    }

    ID3D11Device* GPUDeviceD3D::getNative() const {
        return d3d_device_.get();
    }

}
