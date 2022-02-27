// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "gpu_texture.h"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    //  static
    GPUTexture* GPUTexture::createShaderTex2D(
        uint32_t width, uint32_t height,
        GPUDataFormat format, uint32_t stride, const void* data)
    {
        auto dev = Application::getGraphicDeviceManager()->getGPUDevice();

        Desc desc;
        desc.format = format;
        desc.width = width;
        desc.height = height;
        desc.dim = Dimension::_2D;
        desc.depth = 0u;
        desc.is_dynamic = false;
        desc.mip_levels = 1u;
        desc.res_type = RES_SHADER_RES;

        GPUBuffer::ResourceData data_desc;
        data_desc.pitch = stride;
        data_desc.slice_pitch = 0;
        data_desc.sys_mem = data;

        auto tex = dev->createTexture(&desc, &data_desc);
        if (!tex) {
            return nullptr;
        }

        auto res = dev->createShaderResource(nullptr, tex);
        tex->setShaderRes(res);
        return tex;
    }

    void GPUTexture::setShaderRes(GPUShaderResource* res) {
        shader_res_.reset(res);
    }

    GPUShaderResource* GPUTexture::getShaderRes() const {
        return shader_res_.get();
    }

}
