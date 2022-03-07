// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_texture_metal.h"


namespace ukive {
namespace mac {

    // GPUTexture1DMetal
    GPUTexture1DMetal::GPUTexture1DMetal() {}

    const GPUTexture::Desc& GPUTexture1DMetal::getDesc() const {
        return desc_;
    }


    // GPUTexture2DMetal
    GPUTexture2DMetal::GPUTexture2DMetal() {}

    GPUTexture2DMetal::GPUTexture2DMetal(const Desc& desc)
        : desc_(desc)
    {
    }

    const GPUTexture::Desc& GPUTexture2DMetal::getDesc() const {
        return desc_;
    }


    // GPUTexture3DMetal
    GPUTexture3DMetal::GPUTexture3DMetal(const Desc& desc)
        : desc_(desc)
    {
    }

    const GPUTexture::Desc& GPUTexture3DMetal::getDesc() const {
        return desc_;
    }

}
}
