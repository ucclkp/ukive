// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_TEXTURE_H_
#define UKIVE_GRAPHICS_GPU_GPU_TEXTURE_H_

#include <cstdint>

#include "ukive/graphics/gpu/gpu_resource.h"
#include "ukive/graphics/gpu/gpu_types.h"


namespace ukive {

    class GPUTexture : public GPUResource {
    public:
        enum class Dimension {
            _1D,
            _2D,
            _3D,
        };

        struct Desc {
            Desc()
                : dim(Dimension::_2D),
                  format(GPUDataFormat::UNKNOWN),
                  width(0),
                  height(0),
                  depth(0),
                  mip_levels(0),
                  res_type(0),
                  is_dynamic(false) {}

            Dimension dim;
            GPUDataFormat format;
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mip_levels;
            uint32_t res_type;
            bool is_dynamic;
        };

        virtual ~GPUTexture() = default;

        Type getType() const override { return Type::Texture; }

        virtual const Desc& getDesc() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_TEXTURE_H_