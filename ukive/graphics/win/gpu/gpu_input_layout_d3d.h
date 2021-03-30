// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_INPUT_LAYOUT_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_INPUT_LAYOUT_D3D_H_

#include "ukive/graphics/gpu/gpu_input_layout.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {

    class GPUInputLayoutD3D : public GPUInputLayout {
    public:
        explicit GPUInputLayoutD3D(const ComPtr<ID3D11InputLayout>& il);

        void setNative(const ComPtr<ID3D11InputLayout>& il);
        ID3D11InputLayout* getNative() const;

    private:
        ComPtr<ID3D11InputLayout> d3d_il_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_INPUT_LAYOUT_D3D_H_