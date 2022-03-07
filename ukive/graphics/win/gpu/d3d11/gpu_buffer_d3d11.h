// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_BUFFER_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_BUFFER_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUBufferD3D11 :
        public GPUBuffer,
        public GRefCountImpl
    {
    public:
        GPUBufferD3D11(const utl::win::ComPtr<ID3D11Buffer>& buffer, const Desc& desc);

        const Desc& getDesc() const override;

        ID3D11Buffer* getNative() const;

    private:
        Desc desc_;
        utl::win::ComPtr<ID3D11Buffer> d3d_buffer_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_BUFFER_D3D11_H_