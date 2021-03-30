// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_D3D_UTILS_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_D3D_UTILS_H_

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>

#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_depth_stencil.h"
#include "ukive/graphics/gpu/gpu_depth_stencil_state.h"
#include "ukive/graphics/gpu/gpu_rasterizer_state.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    DXGI_FORMAT mapFormat(GPUDataFormat format);
    D3D_PRIMITIVE_TOPOLOGY mapTopology(GPUContext::Topology t);
    uint32_t mapBindType(uint32_t res_type);
    uint32_t mapCPUAccessFlags(uint32_t flags);

    D3D11_DEPTH_WRITE_MASK mapWriteMask(GPUDepthStencilState::WriteMask mask);
    D3D11_COMPARISON_FUNC mapComparisonFunc(ComparisonFunc func);
    D3D11_STENCIL_OP mapStencilOp(const GPUDepthStencilState::StencilOp& op);

    D3D11_FILL_MODE mapFillMode(GPURasterizerState::FillMode mode);
    D3D11_CULL_MODE mapCullMode(GPURasterizerState::CullMode mode);

    D3D11_FILTER mapFilter(GPUSamplerState::Filter filter);
    D3D11_TEXTURE_ADDRESS_MODE mapAddrMode(GPUSamplerState::TextureAddrMode mode);

    D3D11_DSV_DIMENSION mapDSVDeminsion(GPUDepthStencil::DSVDimension dim);
    D3D11_SRV_DIMENSION mapSRVDeminsion(GPUShaderResource::SRVDimension dim);

}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_D3D_UTILS_H_