// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GPU_GPU_TEXTURE_H_
#define UKIVE_GRAPHICS_GPU_GPU_TEXTURE_H_

#include <cstdint>
#include <memory>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gpu/gpu_resource.h"
#include "ukive/graphics/gpu/gpu_types.h"
#include "ukive/graphics/gpu/gpu_shader_resource.h"


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
                  cpu_access_flags(0),
                  misc_flags(0),
                  usage(GPUDataUsage::Default) {}

            Dimension dim;
            GPUDataFormat format;
            uint32_t width;
            uint32_t height;
            uint32_t depth;  // For 3D texture
            uint32_t mip_levels;
            uint32_t res_type;
            uint32_t cpu_access_flags;
            uint32_t misc_flags;
            GPUDataUsage usage;
        };

        static GEcPtr<GPUTexture> createShaderTex2D(
            uint32_t width, uint32_t height,
            GPUDataFormat format, bool rt);
        static GEcPtr<GPUTexture> createShaderTex2D(
            uint32_t width, uint32_t height,
            GPUDataFormat format, bool rt,
            uint32_t stride, const void* data);
        static GEcPtr<GPUTexture> createStagingTex2D(
            uint32_t width, uint32_t height,
            GPUDataFormat format);

        GPtr<GPUShaderResource> srv() const;

        gerc createSRV();

        Type getType() const override { return Type::Texture; }

        virtual const Desc& getDesc() const = 0;

    private:
        void setSRV(const GPtr<GPUShaderResource>& res);

        GPtr<GPUShaderResource> shader_res_;
    };

}

#endif  // UKIVE_GRAPHICS_GPU_GPU_TEXTURE_H_