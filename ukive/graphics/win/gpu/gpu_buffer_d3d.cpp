// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_buffer_d3d.h"


namespace ukive {

    GPUBufferD3D::GPUBufferD3D(
        const ComPtr<ID3D11Buffer>& buffer, const Desc& desc)
        : desc_(desc), d3d_buffer_(buffer) {}

    const GPUBuffer::Desc& GPUBufferD3D::getDesc() const {
        return desc_;
    }

    ID3D11Buffer* GPUBufferD3D::getNative() const {
        return d3d_buffer_.get();
    }

}
