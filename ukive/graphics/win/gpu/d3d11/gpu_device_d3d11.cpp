// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_device_d3d11.h"

#include <vector>

#include "utils/log.h"
#include "utils/numbers.hpp"

#include "ukive/graphics/win/gpu/d3d11/gpu_buffer_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_input_layout_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_d3d11_utils.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_texture_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_render_target_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_depth_stencil_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_depth_stencil_state_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_rasterizer_state_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_sampler_state_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_shader_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_shader_resource_d3d11.h"


namespace ukive {
namespace win {

    GPUDeviceD3D11::GPUDeviceD3D11(const utl::win::ComPtr<ID3D11Device>& dev)
        : d3d_device_(dev) {}

    GEcPtr<GPUBuffer> GPUDeviceD3D11::createBuffer(
        const GPUBuffer::Desc& desc, const GPUBuffer::ResourceData* data)
    {
        D3D11_BUFFER_DESC vb_desc;
        vb_desc.BindFlags = mapBindType(desc.res_type);
        vb_desc.CPUAccessFlags = mapCPUAccessFlags(desc.cpu_access_flag);
        vb_desc.Usage = desc.is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        vb_desc.ByteWidth = desc.byte_width;
        vb_desc.MiscFlags = 0;
        vb_desc.StructureByteStride = desc.struct_byte_stride;

        HRESULT hr;
        GEcPtr<GPUBuffer> result;
        utl::win::ComPtr<ID3D11Buffer> d3d_buffer;
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
            result.code = hr;
            return result;
        }

        result = new GPUBufferD3D11(d3d_buffer, desc);
        return result;
    }

    GEcPtr<GPUInputLayout> GPUDeviceD3D11::createInputLayout(
        const GPUInputLayout::Desc* desc, size_t desc_count,
        const void* shader_bc, size_t size)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> layouts;
        for (size_t i = 0; i < desc_count; ++i) {
            auto& item = desc[i];

            D3D11_INPUT_ELEMENT_DESC layout;
            layout.SemanticName = item.name;
            layout.SemanticIndex = item.index;
            layout.Format = mapFormat(item.format);
            layout.InputSlot = item.input_slot;
            layout.AlignedByteOffset = item.is_align_append ?
                D3D11_APPEND_ALIGNED_ELEMENT : item.aligned_byte_offset;
            layout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layout.InstanceDataStepRate = 0;

            layouts.push_back(std::move(layout));
        }

        GEcPtr<GPUInputLayout> result;
        utl::win::ComPtr<ID3D11InputLayout> d3d_layout;
        HRESULT hr = d3d_device_->CreateInputLayout(
            layouts.data(), utl::num_cast<UINT>(desc_count), shader_bc, size, &d3d_layout);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUInputLayoutD3D11(d3d_layout);
        return result;
    }

    GEcPtr<GPUTexture> GPUDeviceD3D11::createTexture(
        const GPUTexture::Desc& desc, const GPUBuffer::ResourceData* data)
    {
        GEcPtr<GPUTexture> result;
        D3D11_SUBRESOURCE_DATA res_data;
        if (data) {
            res_data.pSysMem = data->sys_mem;
            res_data.SysMemPitch = data->pitch;
            res_data.SysMemSlicePitch = data->slice_pitch;
        }

        switch (desc.dim) {
        case GPUTexture::Dimension::_1D:
        {
            D3D11_TEXTURE1D_DESC tex_desc;
            tex_desc.ArraySize = 1;
            tex_desc.BindFlags = mapBindType(desc.res_type);
            tex_desc.Format = mapFormat(desc.format);
            tex_desc.Width = desc.width;
            tex_desc.MipLevels = desc.mip_levels;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.Usage = desc.is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
            tex_desc.MiscFlags = 0;

            utl::win::ComPtr<ID3D11Texture1D> t;
            HRESULT hr = d3d_device_->CreateTexture1D(&tex_desc, data ? &res_data : nullptr, &t);
            if (FAILED(hr)) {
                result.code = hr;
                return result;
            }
            result = new GPUTexture1DD3D11(t, desc);
            break;
        }

        case GPUTexture::Dimension::_2D:
        {
            D3D11_TEXTURE2D_DESC tex_desc;
            tex_desc.ArraySize = 1;
            tex_desc.BindFlags = mapBindType(desc.res_type);
            tex_desc.Format = mapFormat(desc.format);
            tex_desc.Width = desc.width;
            tex_desc.Height = desc.height;
            tex_desc.MipLevels = desc.mip_levels;
            tex_desc.SampleDesc.Count = 1;
            tex_desc.SampleDesc.Quality = 0;
            tex_desc.MiscFlags = 0;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.Usage = desc.is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

            utl::win::ComPtr<ID3D11Texture2D> t;
            HRESULT hr = d3d_device_->CreateTexture2D(&tex_desc, data ? &res_data : nullptr, &t);
            if (FAILED(hr)) {
                result.code = hr;
                return result;
            }
            result = new GPUTexture2DD3D11(t, desc);
            break;
        }

        case GPUTexture::Dimension::_3D:
        {
            D3D11_TEXTURE3D_DESC tex_desc;
            tex_desc.BindFlags = mapBindType(desc.res_type);
            tex_desc.Format = mapFormat(desc.format);
            tex_desc.Width = desc.width;
            tex_desc.Height = desc.height;
            tex_desc.Depth = desc.depth;
            tex_desc.MipLevels = desc.mip_levels;
            tex_desc.MiscFlags = 0;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.Usage = desc.is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

            utl::win::ComPtr<ID3D11Texture3D> t;
            HRESULT hr = d3d_device_->CreateTexture3D(&tex_desc, data ? &res_data : nullptr, &t);
            if (FAILED(hr)) {
                result.code = hr;
                return result;
            }
            result = new GPUTexture3DD3D11(t, desc);
            break;
        }
        }
        return result;
    }

    GEcPtr<GPURenderTarget> GPUDeviceD3D11::createRenderTarget(GPUResource* resource) {
        GEcPtr<GPURenderTarget> result;
        ID3D11Resource* d3d_res = convertResource(resource);
        if (!d3d_res) {
            result.code = ERROR_NOT_FOUND;
            return result;
        }

        utl::win::ComPtr<ID3D11RenderTargetView> d3d_rt;
        HRESULT hr = d3d_device_->CreateRenderTargetView(d3d_res, nullptr, &d3d_rt);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPURenderTargetD3D11(d3d_rt);
        return result;
    }

    GEcPtr<GPUDepthStencil> GPUDeviceD3D11::createDepthStencil(
        const GPUDepthStencil::Desc& desc, GPUResource* resource)
    {
        GEcPtr<GPUDepthStencil> result;
        ID3D11Resource* d3d_res = convertResource(resource);
        if (!d3d_res) {
            result.code = ERROR_NOT_FOUND;
            return result;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
        dsv_desc.Format = mapFormat(desc.format);
        dsv_desc.ViewDimension = mapDSVDeminsion(desc.view_dim);
        dsv_desc.Flags = desc.flags;

        switch (desc.view_dim) {
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE1D:
            dsv_desc.Texture1D.MipSlice = desc.tex1d.mip_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE1DARRAY:
            dsv_desc.Texture1DArray.MipSlice = desc.tex1d_array.mip_slice;
            dsv_desc.Texture1DArray.ArraySize = desc.tex1d_array.array_size;
            dsv_desc.Texture1DArray.FirstArraySlice = desc.tex1d_array.first_array_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2D:
            dsv_desc.Texture2D.MipSlice = desc.tex2d.mip_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DARRAY:
            dsv_desc.Texture2DArray.MipSlice = desc.tex2d_array.mip_slice;
            dsv_desc.Texture2DArray.ArraySize = desc.tex2d_array.array_size;
            dsv_desc.Texture2DArray.FirstArraySlice = desc.tex2d_array.first_array_slice;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DMS:
            dsv_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
            break;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DMSARRAY:
            dsv_desc.Texture2DMSArray.ArraySize = desc.tex2dms_array.array_size;
            dsv_desc.Texture2DMSArray.FirstArraySlice = desc.tex2dms_array.first_array_slice;
            break;
        default:
            result.code = ERROR_NOT_FOUND;
            return result;
        }

        utl::win::ComPtr<ID3D11DepthStencilView> d3d_dsv;
        HRESULT hr = d3d_device_->CreateDepthStencilView(d3d_res, &dsv_desc, &d3d_dsv);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUDepthStencilD3D11(d3d_dsv);
        return result;
    }

    GEcPtr<GPUShaderResource> GPUDeviceD3D11::createShaderResource(
        const GPUShaderResource::Desc* desc, GPUResource* resource)
    {
        GEcPtr<GPUShaderResource> result;
        ID3D11Resource* d3d_res = convertResource(resource);
        if (!d3d_res) {
            result.code = ERROR_NOT_FOUND;
            return result;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC* desc_ptr;
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        if (desc) {
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
                result.code = ERROR_NOT_FOUND;
                return result;
            }

            desc_ptr = &srv_desc;
        } else {
            desc_ptr = nullptr;
        }

        utl::win::ComPtr<ID3D11ShaderResourceView> d3d_srv;
        HRESULT hr = d3d_device_->CreateShaderResourceView(d3d_res, desc_ptr, &d3d_srv);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUShaderResourceD3D11(d3d_srv);
        return result;
    }

    GEcPtr<GPUDepthStencilState> GPUDeviceD3D11::createDepthStencilState(const GPUDepthStencilState::Desc& desc) {
        D3D11_DEPTH_STENCIL_DESC dds_desc;
        dds_desc.DepthEnable = desc.depth_enabled ? TRUE : FALSE;
        dds_desc.DepthWriteMask = mapWriteMask(desc.depth_write_mask);
        dds_desc.DepthFunc = mapComparisonFunc(desc.depth_func);
        dds_desc.StencilEnable = desc.stencil_enabled ? TRUE : FALSE;
        dds_desc.StencilReadMask = desc.stencil_read_mask;
        dds_desc.StencilWriteMask = desc.stencil_write_mask;

        dds_desc.FrontFace.StencilFailOp = mapStencilOp(desc.front_face.stencil_fail_op);
        dds_desc.FrontFace.StencilDepthFailOp = mapStencilOp(desc.front_face.stencil_depth_fail_op);
        dds_desc.FrontFace.StencilPassOp = mapStencilOp(desc.front_face.stencil_pass_op);
        dds_desc.FrontFace.StencilFunc = mapComparisonFunc(desc.front_face.stencil_func);

        dds_desc.BackFace.StencilFailOp = mapStencilOp(desc.back_face.stencil_fail_op);
        dds_desc.BackFace.StencilDepthFailOp = mapStencilOp(desc.back_face.stencil_depth_fail_op);
        dds_desc.BackFace.StencilPassOp = mapStencilOp(desc.back_face.stencil_pass_op);
        dds_desc.BackFace.StencilFunc = mapComparisonFunc(desc.back_face.stencil_func);

        GEcPtr<GPUDepthStencilState> result;
        utl::win::ComPtr<ID3D11DepthStencilState> dds_state;
        HRESULT hr = d3d_device_->CreateDepthStencilState(&dds_desc, &dds_state);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUDepthStencilStateD3D11(dds_state);
        return result;
    }

    GEcPtr<GPURasterizerState> GPUDeviceD3D11::createRasterizerState(const GPURasterizerState::Desc& desc) {
        D3D11_RASTERIZER_DESC r_desc;
        r_desc.FillMode = mapFillMode(desc.fill_mode);
        r_desc.CullMode = mapCullMode(desc.cull_mode);
        r_desc.FrontCounterClockwise = desc.front_counter_clockwise ? TRUE : FALSE;
        r_desc.DepthBias = 0;
        r_desc.DepthBiasClamp = 0;
        r_desc.SlopeScaledDepthBias = 0;
        r_desc.DepthClipEnable = desc.depth_clip_enabled ? TRUE : FALSE;
        r_desc.ScissorEnable = desc.scissor_enabled ? TRUE : FALSE;
        r_desc.MultisampleEnable = desc.multisample_enabled ? TRUE : FALSE;
        r_desc.AntialiasedLineEnable = desc.antialiased_line_enabled ? TRUE : FALSE;

        GEcPtr<GPURasterizerState> result;
        utl::win::ComPtr<ID3D11RasterizerState> rs;
        HRESULT hr = d3d_device_->CreateRasterizerState(&r_desc, &rs);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPURasterizerStateD3D11(rs);
        return result;
    }

    GEcPtr<GPUSamplerState> GPUDeviceD3D11::createSamplerState(const GPUSamplerState::Desc& desc) {
        D3D11_SAMPLER_DESC s_desc;
        s_desc.Filter = mapFilter(desc.filter);
        s_desc.AddressU = mapAddrMode(desc.addr_u);
        s_desc.AddressV = mapAddrMode(desc.addr_v);
        s_desc.AddressW = mapAddrMode(desc.addr_w);
        s_desc.MipLODBias = desc.mip_lod_bias;
        s_desc.MaxAnisotropy = desc.max_anisotropy;
        s_desc.ComparisonFunc = mapComparisonFunc(desc.comp_func);
        std::memcpy(s_desc.BorderColor, desc.border_color, sizeof(float) * 4);
        s_desc.MinLOD = desc.min_lod;
        s_desc.MaxLOD = desc.max_lod;

        GEcPtr<GPUSamplerState> result;
        utl::win::ComPtr<ID3D11SamplerState> ss;
        HRESULT hr = d3d_device_->CreateSamplerState(&s_desc, &ss);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUSamplerStateD3D11(ss);
        return result;
    }

    GEcPtr<GPUShader> GPUDeviceD3D11::createVertexShader(const void* shader_bc, size_t size) {
        GEcPtr<GPUShader> result;
        utl::win::ComPtr<ID3D11VertexShader> vs;
        HRESULT hr = d3d_device_->CreateVertexShader(shader_bc, size, nullptr, &vs);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUVertexShaderD3D11(vs);
        return result;
    }

    GEcPtr<GPUShader> GPUDeviceD3D11::createPixelShader(const void* shader_bc, size_t size) {
        GEcPtr<GPUShader> result;
        utl::win::ComPtr<ID3D11PixelShader> ps;
        HRESULT hr = d3d_device_->CreatePixelShader(shader_bc, size, nullptr, &ps);
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUPixelShaderD3D11(ps);
        return result;
    }

    GEcPtr<GPUTexture> GPUDeviceD3D11::openSharedTexture2D(intptr_t handle) {
        GEcPtr<GPUTexture> result;
        utl::win::ComPtr<ID3D11Texture2D> tex2d;
        HRESULT hr = d3d_device_->OpenSharedResource(
            reinterpret_cast<HANDLE>(handle), IID_PPV_ARGS(&tex2d));
        if (FAILED(hr)) {
            result.code = hr;
            return result;
        }

        result = new GPUTexture2DD3D11(tex2d);
        return result;
    }

    ID3D11Resource* GPUDeviceD3D11::convertResource(GPUResource* res) const {
        ID3D11Resource* d3d_res = nullptr;
        switch (res->getType()) {
        case GPUResource::Type::Buffer:
            d3d_res = static_cast<GPUBufferD3D11*>(res)->getNative();
            break;
        case GPUResource::Type::Texture:
        {
            auto& tex_desc = static_cast<GPUTexture*>(res)->getDesc();
            switch (tex_desc.dim) {
            case GPUTexture::Dimension::_1D:
                d3d_res = static_cast<GPUTexture1DD3D11*>(res)->getNative().get();
                break;
            case GPUTexture::Dimension::_2D:
                d3d_res = static_cast<GPUTexture2DD3D11*>(res)->getNative().get();
                break;
            case GPUTexture::Dimension::_3D:
                d3d_res = static_cast<GPUTexture3DD3D11*>(res)->getNative().get();
                break;
            }
            break;
        }
        }
        return d3d_res;
    }

    utl::win::ComPtr<ID3D11Device> GPUDeviceD3D11::getNative() const {
        return d3d_device_;
    }

}
}