// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_NATIVE_RT_D2D_H_
#define UKIVE_GRAPHICS_WIN_NATIVE_RT_D2D_H_

#include "ukive/graphics/native_rt.h"

#include "utils/memory/win/com_ptr.hpp"

#include <d2d1.h>


namespace ukive {
namespace win {

    class NativeRTD2D : public NativeRT {
    public:
        NativeRTD2D() = default;

        void destroy() override;

        void setNative(const utl::win::ComPtr<ID2D1RenderTarget>& rt);
        utl::win::ComPtr<ID2D1RenderTarget> getNative() const;

    private:
        utl::win::ComPtr<ID2D1RenderTarget> rt_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_NATIVE_RT_D2D_H_