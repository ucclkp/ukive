// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_texture_d3d.h"


namespace ukive {
namespace win {

    // GPUTexture1DD3D
    GPUTexture1DD3D::GPUTexture1DD3D(const ComPtr<ID3D11Texture1D>& t, const Desc& desc)
        : desc_(desc), t1d_(t) {}

    const GPUTexture::Desc& GPUTexture1DD3D::getDesc() const {
        return desc_;
    }

    ComPtr<ID3D11Texture1D> GPUTexture1DD3D::getNative() const {
        return t1d_;
    }


    // GPUTexture2DD3D
    GPUTexture2DD3D::GPUTexture2DD3D(const ComPtr<ID3D11Texture2D>& t, const Desc& desc)
        : desc_(desc), t2d_(t) {}

    const GPUTexture::Desc& GPUTexture2DD3D::getDesc() const {
        return desc_;
    }

    ComPtr<ID3D11Texture2D> GPUTexture2DD3D::getNative() const {
        return t2d_;
    }


    // GPUTexture3DD3D
    GPUTexture3DD3D::GPUTexture3DD3D(const ComPtr<ID3D11Texture3D>& t, const Desc& desc)
        : desc_(desc), t3d_(t) {}

    const GPUTexture::Desc& GPUTexture3DD3D::getDesc() const {
        return desc_;
    }

    ComPtr<ID3D11Texture3D> GPUTexture3DD3D::getNative() const {
        return t3d_;
    }

}
}