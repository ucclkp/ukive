// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_GPU_GPU_TEXTURE_D3D_H_
#define UKIVE_GRAPHICS_WIN_GPU_GPU_TEXTURE_D3D_H_

#include "ukive/graphics/gpu/gpu_texture.h"
#include "ukive/system/win/com_ptr.hpp"

#include <d3d11.h>


namespace ukive {

    class GPUTexture1DD3D : public GPUTexture {
    public:
        explicit GPUTexture1DD3D(
            const ComPtr<ID3D11Texture1D>& t, const Desc& desc);

        const Desc& getDesc() const override;

        ComPtr<ID3D11Texture1D> getNative() const;

    private:
        Desc desc_;
        ComPtr<ID3D11Texture1D> t1d_;
    };


    class GPUTexture2DD3D : public GPUTexture {
    public:
        explicit GPUTexture2DD3D(
            const ComPtr<ID3D11Texture2D>& t, const Desc& desc);

        const Desc& getDesc() const override;

        ComPtr<ID3D11Texture2D> getNative() const;

    private:
        Desc desc_;
        ComPtr<ID3D11Texture2D> t2d_;
    };


    class GPUTexture3DD3D : public GPUTexture {
    public:
        explicit GPUTexture3DD3D(
            const ComPtr<ID3D11Texture3D>& t, const Desc& desc);

        const Desc& getDesc() const override;

        ComPtr<ID3D11Texture3D> getNative() const;

    private:
        Desc desc_;
        ComPtr<ID3D11Texture3D> t3d_;
    };

}

#endif  // UKIVE_GRAPHICS_WIN_GPU_GPU_TEXTURE_D3D_H_