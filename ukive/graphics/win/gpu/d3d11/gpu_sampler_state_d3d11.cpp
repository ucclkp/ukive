// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_sampler_state_d3d11.h"


namespace ukive {
namespace win {

    GPUSamplerStateD3D11::GPUSamplerStateD3D11(
        const utl::win::ComPtr<ID3D11SamplerState>& ss)
        : ss_(ss) {}

    void GPUSamplerStateD3D11::setNative(const utl::win::ComPtr<ID3D11SamplerState>& ss) {
        ss_ = ss;
    }

    ID3D11SamplerState* GPUSamplerStateD3D11::getNative() const {
        return ss_.get();
    }

}
}