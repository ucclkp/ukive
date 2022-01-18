// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_d3d_utils.h"

#include "utils/log.h"

#include <d3d11.h>


namespace ukive {

    DXGI_FORMAT mapFormat(GPUDataFormat format) {
        switch (format) {
        case GPUDataFormat::R32G32B32A32_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case GPUDataFormat::R32G32B32A32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case GPUDataFormat::R32G32B32_FLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case GPUDataFormat::R32G32B32_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;
        case GPUDataFormat::R32G32_FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case GPUDataFormat::R16G16B16A16_FLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case GPUDataFormat::R16G16B16A16_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case GPUDataFormat::R8G8B8A8_UINT:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case GPUDataFormat::R32_UINT:
            return DXGI_FORMAT_R32_UINT;
        case GPUDataFormat::B8G8B8R8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case GPUDataFormat::D24_UNORM_S8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case GPUDataFormat::UNKNOWN:
            return DXGI_FORMAT_UNKNOWN;
        default:
            ubassert(false);
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    D3D_PRIMITIVE_TOPOLOGY mapTopology(GPUContext::Topology t) {
        switch (t) {
        case GPUContext::Topology::PointList:
            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case GPUContext::Topology::LineList:
            return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case GPUContext::Topology::LineStrip:
            return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case GPUContext::Topology::TriangleList:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case GPUContext::Topology::TriangleStrip:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            ubassert(false);
            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        }
    }

    uint32_t mapBindType(uint32_t res_type) {
        uint32_t bind_flags = 0;
        if (res_type & GPUResource::RES_VERTEX_BUFFER) {
            bind_flags |= D3D11_BIND_VERTEX_BUFFER;
        }
        if (res_type & GPUResource::RES_INDEX_BUFFER) {
            bind_flags |= D3D11_BIND_INDEX_BUFFER;
        }
        if (res_type & GPUResource::RES_CONSTANT_BUFFER) {
            bind_flags |= D3D11_BIND_CONSTANT_BUFFER;
        }
        if (res_type & GPUResource::RES_DEPTH_STENCIL) {
            bind_flags |= D3D11_BIND_DEPTH_STENCIL;
        }
        if (res_type & GPUResource::RES_RENDER_TARGET) {
            bind_flags |= D3D11_BIND_RENDER_TARGET;
        }
        if (res_type & GPUResource::RES_SHADER_RES) {
            bind_flags |= D3D11_BIND_SHADER_RESOURCE;
        }
        return bind_flags;
    }

    uint32_t mapCPUAccessFlags(uint32_t flags) {
        uint32_t result = 0;
        if (flags & GPUResource::CPU_ACCESS_READ) {
            result |= D3D11_CPU_ACCESS_READ;
        }
        if (flags & GPUResource::CPU_ACCESS_WRITE) {
            result |= D3D11_CPU_ACCESS_WRITE;
        }
        return result;
    }

    D3D11_DEPTH_WRITE_MASK mapWriteMask(GPUDepthStencilState::WriteMask mask) {
        switch (mask) {
        case GPUDepthStencilState::WriteMask::All:
            return D3D11_DEPTH_WRITE_MASK_ALL;
        case GPUDepthStencilState::WriteMask::Zero:
            return D3D11_DEPTH_WRITE_MASK_ZERO;
        }
        ubassert(false);
        return D3D11_DEPTH_WRITE_MASK_ZERO;
    }

    D3D11_COMPARISON_FUNC mapComparisonFunc(ComparisonFunc func) {
        switch (func) {
        case ComparisonFunc::Never:
            return D3D11_COMPARISON_NEVER;
        case ComparisonFunc::Less:
            return D3D11_COMPARISON_LESS;
        case ComparisonFunc::Equal:
            return D3D11_COMPARISON_EQUAL;
        case ComparisonFunc::LessEqual:
            return D3D11_COMPARISON_LESS_EQUAL;
        case ComparisonFunc::Greater:
            return D3D11_COMPARISON_GREATER;
        case ComparisonFunc::NotEqual:
            return D3D11_COMPARISON_NOT_EQUAL;
        case ComparisonFunc::GreaterEqual:
            return D3D11_COMPARISON_GREATER_EQUAL;
        case ComparisonFunc::Always:
            return D3D11_COMPARISON_ALWAYS;
        }
        ubassert(false);
        return D3D11_COMPARISON_NEVER;
    }

    D3D11_STENCIL_OP mapStencilOp(const GPUDepthStencilState::StencilOp& op) {
        switch (op) {
        case GPUDepthStencilState::StencilOp::Keep:
            return D3D11_STENCIL_OP_KEEP;
        case GPUDepthStencilState::StencilOp::Zero:
            return D3D11_STENCIL_OP_ZERO;
        case GPUDepthStencilState::StencilOp::Replace:
            return D3D11_STENCIL_OP_REPLACE;
        case GPUDepthStencilState::StencilOp::IncrSat:
            return D3D11_STENCIL_OP_INCR_SAT;
        case GPUDepthStencilState::StencilOp::DecrSat:
            return D3D11_STENCIL_OP_DECR_SAT;
        case GPUDepthStencilState::StencilOp::Invert:
            return D3D11_STENCIL_OP_INVERT;
        case GPUDepthStencilState::StencilOp::Incr:
            return D3D11_STENCIL_OP_INCR;
        case GPUDepthStencilState::StencilOp::Decr:
            return D3D11_STENCIL_OP_DECR;
        }
        ubassert(false);
        return D3D11_STENCIL_OP_KEEP;
    }

    D3D11_FILL_MODE mapFillMode(GPURasterizerState::FillMode mode) {
        switch (mode) {
        case GPURasterizerState::FillMode::Solid:
            return D3D11_FILL_SOLID;
        case GPURasterizerState::FillMode::WireFrame:
            return D3D11_FILL_WIREFRAME;
        }
        ubassert(false);
        return D3D11_FILL_WIREFRAME;
    }

    D3D11_CULL_MODE mapCullMode(GPURasterizerState::CullMode mode) {
        switch (mode) {
        case GPURasterizerState::CullMode::None:
            return D3D11_CULL_NONE;
        case GPURasterizerState::CullMode::Back:
            return D3D11_CULL_BACK;
        case GPURasterizerState::CullMode::Front:
            return D3D11_CULL_FRONT;
        }
        ubassert(false);
        return D3D11_CULL_NONE;
    }

    D3D11_FILTER mapFilter(GPUSamplerState::Filter filter) {
        switch (filter) {
        case GPUSamplerState::Filter::MinMagMipPoint:
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        case GPUSamplerState::Filter::MinMagPointMipLinear:
            return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        }
        ubassert(false);
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
    }

    D3D11_TEXTURE_ADDRESS_MODE mapAddrMode(GPUSamplerState::TextureAddrMode mode) {
        switch (mode) {
        case GPUSamplerState::TextureAddrMode::Wrap:
            return D3D11_TEXTURE_ADDRESS_WRAP;
        case GPUSamplerState::TextureAddrMode::Mirror:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        case GPUSamplerState::TextureAddrMode::Clamp:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        case GPUSamplerState::TextureAddrMode::Border:
            return D3D11_TEXTURE_ADDRESS_BORDER;
        case GPUSamplerState::TextureAddrMode::MirrorOnce:
            return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
        }
        ubassert(false);
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    }

    D3D11_DSV_DIMENSION mapDSVDeminsion(GPUDepthStencil::DSVDimension dim) {
        switch (dim) {
        case GPUDepthStencil::DSV_DIMENSION_UNKNOWN:
            return D3D11_DSV_DIMENSION_UNKNOWN;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE1D:
            return D3D11_DSV_DIMENSION_TEXTURE1D;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE1DARRAY:
            return D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2D:
            return D3D11_DSV_DIMENSION_TEXTURE2D;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DARRAY:
            return D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DMS:
            return D3D11_DSV_DIMENSION_TEXTURE2DMS;
        case GPUDepthStencil::DSV_DIMENSION_TEXTURE2DMSARRAY:
            return D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
        default:
            break;
        }
        ubassert(false);
        return D3D11_DSV_DIMENSION_UNKNOWN;
    }

    D3D11_SRV_DIMENSION mapSRVDeminsion(GPUShaderResource::SRVDimension dim) {
        switch (dim) {
        case GPUShaderResource::SRV_DIMENSION_UNKNOWN:
            return D3D11_SRV_DIMENSION_UNKNOWN;
        case GPUShaderResource::SRV_DIMENSION_BUFFER:
            return D3D11_SRV_DIMENSION_BUFFER;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE1D:
            return D3D11_SRV_DIMENSION_TEXTURE1D;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE1DARRAY:
            return D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2D:
            return D3D11_SRV_DIMENSION_TEXTURE2D;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2DARRAY:
            return D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2DMS:
            return D3D11_SRV_DIMENSION_TEXTURE2DMS;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE2DMSARRAY:
            return D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
        case GPUShaderResource::SRV_DIMENSION_TEXTURE3D:
            return D3D11_SRV_DIMENSION_TEXTURE3D;
        case GPUShaderResource::SRV_DIMENSION_TEXTURECUBE:
            return D3D11_SRV_DIMENSION_TEXTURECUBE;
        case GPUShaderResource::SRV_DIMENSION_TEXTURECUBEARRAY:
            return D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
        case GPUShaderResource::SRV_DIMENSION_BUFFEREX:
            return D3D11_SRV_DIMENSION_BUFFEREX;
        default:
            break;
        }

        ubassert(false);
        return D3D11_SRV_DIMENSION_UNKNOWN;
    }

}
