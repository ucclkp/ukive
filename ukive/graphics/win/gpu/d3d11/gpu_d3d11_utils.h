// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_D3D11_UTILS_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_D3D11_UTILS_H_

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>

#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_depth_stencil.h"
#include "ukive/graphics/gpu/gpu_depth_stencil_state.h"
#include "ukive/graphics/gpu/gpu_rasterizer_state.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {
namespace win {

    DXGI_FORMAT mapDXGIFormat(GPUDataFormat format);
    GPUDataFormat unmapDXGIFormat(DXGI_FORMAT format);

    D3D11_USAGE mapD3DUsage(GPUDataUsage usage);
    GPUDataUsage unmapD3DUsage(D3D11_USAGE usage);

    uint32_t mapD3DCPUAccessFlags(uint32_t flags);
    uint32_t unmapD3DCPUAccessFlags(uint32_t d3d_flags);

    uint32_t mapD3DResMiscFlags(uint32_t flags);
    uint32_t unmapD3DResMiscFlags(uint32_t d3d_flags);

    D3D_PRIMITIVE_TOPOLOGY mapD3DTopology(GPUContext::Topology t);

    uint32_t mapD3DBindType(uint32_t res_type);
    uint32_t unmapD3DBindType(uint32_t bind_flags);

    D3D11_DEPTH_WRITE_MASK mapD3DWriteMask(GPUDepthStencilState::WriteMask mask);
    D3D11_COMPARISON_FUNC mapD3DComparisonFunc(ComparisonFunc func);
    D3D11_STENCIL_OP mapD3DStencilOp(const GPUDepthStencilState::StencilOp& op);

    D3D11_FILL_MODE mapD3DFillMode(GPURasterizerState::FillMode mode);
    D3D11_CULL_MODE mapD3DCullMode(GPURasterizerState::CullMode mode);

    D3D11_FILTER mapD3DFilter(GPUSamplerState::Filter filter);
    D3D11_TEXTURE_ADDRESS_MODE mapD3DAddrMode(GPUSamplerState::TextureAddrMode mode);

    D3D11_DSV_DIMENSION mapD3DDSVDeminsion(GPUDepthStencil::DSVDimension dim);
    D3D11_SRV_DIMENSION mapD3DSRVDeminsion(GPUShaderResource::SRVDimension dim);

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_D3D11_UTILS_H_