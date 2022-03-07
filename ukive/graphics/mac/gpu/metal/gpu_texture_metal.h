// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_TEXTURE_METAL_H_
#define UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_TEXTURE_METAL_H_

#include "ukive/graphics/gpu/gpu_texture.h"
#include "ukive/graphics/gref_count_impl.h"


namespace ukive {
namespace mac {

    class GPUTexture1DMetal :
        public GPUTexture,
        public GRefCountImpl
    {
    public:
        explicit GPUTexture1DMetal();

        const Desc& getDesc() const override;

    private:
        Desc desc_;
    };


    class GPUTexture2DMetal :
        public GPUTexture,
        public GRefCountImpl
    {
    public:
        explicit GPUTexture2DMetal();
        GPUTexture2DMetal(const Desc& desc);

        const Desc& getDesc() const override;

    private:
        Desc desc_;
    };


    class GPUTexture3DMetal :
        public GPUTexture,
        public GRefCountImpl
    {
    public:
        explicit GPUTexture3DMetal(const Desc& desc);

        const Desc& getDesc() const override;

    private:
        Desc desc_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_GPU_METAL_GPU_TEXTURE_METAL_H_