// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_render_target_d3d11.h"


namespace ukive {
namespace win {

    GPURenderTargetD3D11::GPURenderTargetD3D11(const utl::win::ComPtr<ID3D11RenderTargetView>& rtv)
        : rtv_(rtv) {}

    GPURenderTarget::Native GPURenderTargetD3D11::getOpaque() const {
        return reinterpret_cast<Native>(rtv_.get());
    }

    void GPURenderTargetD3D11::setNative(const utl::win::ComPtr<ID3D11RenderTargetView>& rtv) {
        rtv_ = rtv;
    }

    ID3D11RenderTargetView* GPURenderTargetD3D11::getNative() const {
        return rtv_.get();
    }

}
}