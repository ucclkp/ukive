// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_input_layout_d3d11.h"


namespace ukive {
namespace win {

    GPUInputLayoutD3D11::GPUInputLayoutD3D11(const utl::win::ComPtr<ID3D11InputLayout>& il)
        : d3d_il_(il) {}

    void GPUInputLayoutD3D11::setNative(const utl::win::ComPtr<ID3D11InputLayout>& il) {
        d3d_il_ = il;
    }

    ID3D11InputLayout* GPUInputLayoutD3D11::getNative() const {
        return d3d_il_.get();
    }

}
}