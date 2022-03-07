// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_texture_d3d11.h"
#include "ukive/graphics/win/gpu/d3d11/gpu_d3d11_utils.h"


namespace ukive {
namespace win {

    // GPUTexture1DD3D11
    GPUTexture1DD3D11::GPUTexture1DD3D11(
        const utl::win::ComPtr<ID3D11Texture1D>& t, const Desc& desc)
        : desc_(desc), t1d_(t) {}

    const GPUTexture::Desc& GPUTexture1DD3D11::getDesc() const {
        return desc_;
    }

    utl::win::ComPtr<ID3D11Texture1D> GPUTexture1DD3D11::getNative() const {
        return t1d_;
    }


    // GPUTexture2DD3D11
    GPUTexture2DD3D11::GPUTexture2DD3D11(
        const utl::win::ComPtr<ID3D11Texture2D>& t)
        : t2d_(t)
    {
        D3D11_TEXTURE2D_DESC raw_desc;
        t2d_->GetDesc(&raw_desc);

        desc_.dim = Dimension::_2D;
        desc_.format = unmapFormat(raw_desc.Format);
        desc_.width = raw_desc.Width;
        desc_.height = raw_desc.Height;
        desc_.depth = 0;
        desc_.mip_levels = raw_desc.MipLevels;
        desc_.res_type = unmapBindType(raw_desc.BindFlags);
        desc_.is_dynamic = raw_desc.Usage == D3D11_USAGE_DYNAMIC;
    }

    GPUTexture2DD3D11::GPUTexture2DD3D11(
        const utl::win::ComPtr<ID3D11Texture2D>& t, const Desc& desc)
        : desc_(desc), t2d_(t) {}

    const GPUTexture::Desc& GPUTexture2DD3D11::getDesc() const {
        return desc_;
    }

    utl::win::ComPtr<ID3D11Texture2D> GPUTexture2DD3D11::getNative() const {
        return t2d_;
    }


    // GPUTexture3DD3D11
    GPUTexture3DD3D11::GPUTexture3DD3D11(
        const utl::win::ComPtr<ID3D11Texture3D>& t, const Desc& desc)
        : desc_(desc), t3d_(t) {}

    const GPUTexture::Desc& GPUTexture3DD3D11::getDesc() const {
        return desc_;
    }

    utl::win::ComPtr<ID3D11Texture3D> GPUTexture3DD3D11::getNative() const {
        return t3d_;
    }

}
}