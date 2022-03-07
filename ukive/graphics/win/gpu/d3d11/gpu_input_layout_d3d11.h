// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_INPUT_LAYOUT_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_INPUT_LAYOUT_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_input_layout.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUInputLayoutD3D11 :
        public GPUInputLayout,
        public GRefCountImpl
    {
    public:
        explicit GPUInputLayoutD3D11(const utl::win::ComPtr<ID3D11InputLayout>& il);

        void setNative(const utl::win::ComPtr<ID3D11InputLayout>& il);
        ID3D11InputLayout* getNative() const;

    private:
        utl::win::ComPtr<ID3D11InputLayout> d3d_il_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_INPUT_LAYOUT_D3D11_H_