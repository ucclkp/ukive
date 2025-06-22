// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_output_resource_d3d11.h"


namespace ukive {
namespace win {

    GPUOutputResourceD3D11::GPUOutputResourceD3D11(
        const utl::win::ComPtr<ID3D11UnorderedAccessView>& uav)
        : uav_(uav) {}

    void GPUOutputResourceD3D11::setNative(const utl::win::ComPtr<ID3D11UnorderedAccessView>& uav) {
        uav_ = uav;
    }

    ID3D11UnorderedAccessView* GPUOutputResourceD3D11::getNative() const {
        return uav_.get();
    }

}
}