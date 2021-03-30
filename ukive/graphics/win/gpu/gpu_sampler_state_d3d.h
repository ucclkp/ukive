// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_SAMPLER_STATE_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_SAMPLER_STATE_D3D_H_

#include "ukive/graphics/gpu/gpu_sampler_state.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {

    class GPUSamplerStateD3D : public GPUSamplerState {
    public:
        explicit GPUSamplerStateD3D(const ComPtr<ID3D11SamplerState>& ss);

        void setNative(const ComPtr<ID3D11SamplerState>& ss);
        ID3D11SamplerState* getNative() const;

    private:
        ComPtr<ID3D11SamplerState> ss_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_SAMPLER_STATE_D3D_H_