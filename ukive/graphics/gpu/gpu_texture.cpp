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
    GEcPtr<GPUTexture> GPUTexture::createShaderTex2D(
        uint32_t width, uint32_t height,
        GPUDataFormat format, bool rt)
    {
        return createShaderTex2D(width, height, format, rt, 0, nullptr);
    }

    //  static
    GEcPtr<GPUTexture> GPUTexture::createShaderTex2D(
        uint32_t width, uint32_t height,
        GPUDataFormat format, bool rt,
        uint32_t stride, const void* data)
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
        desc.res_type = RES_SHADER_RES | (rt ? RES_RENDER_TARGET : 0);

        GPUBuffer::ResourceData data_desc;
        if (data) {
            data_desc.pitch = stride;
            data_desc.slice_pitch = 0;
            data_desc.sys_mem = data;
        }

        auto tex = dev->createTexture(desc, data ? &data_desc : nullptr);
        if (!tex) {
            return tex;
        }

        tex.code = tex->createSRV();
        return tex;
    }

    void GPUTexture::setSRV(const GPtr<GPUShaderResource>& res) {
        shader_res_ = res;
    }

    GPtr<GPUShaderResource> GPUTexture::srv() const {
        return shader_res_;
    }

    gerc GPUTexture::createSRV() {
        shader_res_.reset();
        auto dev = Application::getGraphicDeviceManager()->getGPUDevice();
        auto srv = dev->createShaderResource(nullptr, this);
        setSRV(srv);
        return srv.code;
    }

}
