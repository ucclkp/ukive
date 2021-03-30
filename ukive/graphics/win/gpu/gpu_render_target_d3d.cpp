// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/gpu/gpu_render_target_d3d.h"


namespace ukive {

    GPURenderTargetD3D::GPURenderTargetD3D(const ComPtr<ID3D11RenderTargetView>& rtv)
        : rtv_(rtv) {}

    GPURenderTarget::Native GPURenderTargetD3D::getOpaque() const {
        return reinterpret_cast<Native>(rtv_.get());
    }

    void GPURenderTargetD3D::setNative(const ComPtr<ID3D11RenderTargetView>& rtv) {
        rtv_ = rtv;
    }

    ID3D11RenderTargetView* GPURenderTargetD3D::getNative() const {
        return rtv_.get();
    }

}
