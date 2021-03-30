// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/cyro_renderer.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/cyro_renderer_d2d.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/cyro_renderer_mac.h"
#endif


namespace ukive {

    // static
    CyroRenderer* CyroRenderer::create() {
#ifdef OS_WINDOWS
        return new CyroRendererD2D();
#elif defined OS_MAC
        return new CyroRendererMac();
#endif
    }

}
