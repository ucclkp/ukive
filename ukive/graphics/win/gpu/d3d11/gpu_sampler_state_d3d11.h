// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_SAMPLER_STATE_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_SAMPLER_STATE_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_sampler_state.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUSamplerStateD3D11 :
        public GPUSamplerState,
        public GRefCountImpl
    {
    public:
        explicit GPUSamplerStateD3D11(const utl::win::ComPtr<ID3D11SamplerState>& ss);

        void setNative(const utl::win::ComPtr<ID3D11SamplerState>& ss);
        ID3D11SamplerState* getNative() const;

    private:
        utl::win::ComPtr<ID3D11SamplerState> ss_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_SAMPLER_STATE_D3D11_H_