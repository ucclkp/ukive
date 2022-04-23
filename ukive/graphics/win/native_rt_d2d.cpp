// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/native_rt_d2d.h"


namespace ukive {
namespace win {

    void NativeRTD2D::destroy() {
        rt_.reset();
    }

    void NativeRTD2D::setNative(const utl::win::ComPtr<ID2D1RenderTarget>& rt) {
        rt_ = rt;
    }

    utl::win::ComPtr<ID2D1RenderTarget> NativeRTD2D::getNative() const {
        return rt_;
    }

}
}