// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_buffer_metal.h"


namespace ukive {
namespace mac {

    GPUBufferMetal::GPUBufferMetal(const Desc& desc)
        : desc_(desc) {}

    const GPUBuffer::Desc& GPUBufferMetal::getDesc() const {
        return desc_;
    }

}
}