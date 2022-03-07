// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_render_target_metal.h"


namespace ukive {
namespace mac {

    GPURenderTargetMetal::GPURenderTargetMetal() {}

    GPURenderTarget::Native GPURenderTargetMetal::getOpaque() const {
        return 0;
    }

}
}
