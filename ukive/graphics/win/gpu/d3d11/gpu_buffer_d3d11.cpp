// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_buffer_d3d11.h"


namespace ukive {
namespace win {

    GPUBufferD3D11::GPUBufferD3D11(
        const utl::win::ComPtr<ID3D11Buffer>& buffer, const Desc& desc)
        : desc_(desc), d3d_buffer_(buffer) {}

    const GPUBuffer::Desc& GPUBufferD3D11::getDesc() const {
        return desc_;
    }

    ID3D11Buffer* GPUBufferD3D11::getNative() const {
        return d3d_buffer_.get();
    }

}
}