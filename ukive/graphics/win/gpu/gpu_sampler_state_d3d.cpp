// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_sampler_state_d3d.h"


namespace ukive {
namespace win {

    GPUSamplerStateD3D::GPUSamplerStateD3D(
        const ComPtr<ID3D11SamplerState>& ss)
        : ss_(ss) {}

    void GPUSamplerStateD3D::setNative(const ComPtr<ID3D11SamplerState>& ss) {
        ss_ = ss;
    }

    ID3D11SamplerState* GPUSamplerStateD3D::getNative() const {
        return ss_.get();
    }

}
}