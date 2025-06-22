// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_OUTPUT_RESOURCE_D3D11_H_
#define UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_OUTPUT_RESOURCE_D3D11_H_

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/gpu/gpu_output_resource.h"
#include "ukive/graphics/gref_count_impl.h"

#include <d3d11.h>


namespace ukive {
namespace win {

    class GPUOutputResourceD3D11 :
        public GPUOutputResource,
        public GRefCountImpl
    {
    public:
        explicit GPUOutputResourceD3D11(
            const utl::win::ComPtr<ID3D11UnorderedAccessView>& uav);

        void setNative(const utl::win::ComPtr<ID3D11UnorderedAccessView>& uav);
        ID3D11UnorderedAccessView* getNative() const;

    private:
        utl::win::ComPtr<ID3D11UnorderedAccessView> uav_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_GPU_D3D11_GPU_OUTPUT_RESOURCE_D3D11_H_