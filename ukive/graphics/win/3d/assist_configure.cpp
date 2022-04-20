// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "assist_configure.h"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gpu/gpu_input_layout.h"
#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/resources/resource_manager.h"


namespace ukive {

    AssistConfigure::AssistConfigure() {}

    AssistConfigure::~AssistConfigure() {}

    void AssistConfigure::init() {
        auto res_mgr = Application::getResourceManager();
        auto gpu_device = Application::getGraphicDeviceManager()->getGPUDevice();

        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string vs_bc;
        res_mgr->getFileData(shader_dir / u"assist_vertex_shader.cso", &vs_bc);
        assist_vs_ = gpu_device->createVertexShader(vs_bc.data(), vs_bc.size());

        std::string ps_bc;
        res_mgr->getFileData(shader_dir / u"assist_pixel_shader.cso", &ps_bc);
        assist_ps_ = gpu_device->createPixelShader(ps_bc.data(), ps_bc.size());

        using GILDesc = GPUInputLayout::Desc;
        GILDesc desc[3];
        desc[0] = GILDesc("POSITION", GPUDataFormat::R32G32B32_FLOAT, 0, 0, false);
        desc[1] = GILDesc("COLOR", GPUDataFormat::R32G32B32A32_FLOAT);
        desc[2] = GILDesc("TEXCOORD", GPUDataFormat::R32G32_FLOAT);
        assist_il_ = gpu_device->createInputLayout(desc, 3, vs_bc.data(), vs_bc.size());

        GPUBuffer::Desc buf_desc;
        buf_desc.is_dynamic = true;
        buf_desc.byte_width = sizeof(AssistConstBuffer);
        buf_desc.res_type = GPUResource::RES_CONSTANT_BUFFER;
        buf_desc.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
        buf_desc.struct_byte_stride = 0;
        assist_cb_ = gpu_device->createBuffer(buf_desc, nullptr);
    }

    void AssistConfigure::active(GPUContext* context) {
        context->setVertexShader(assist_vs_.get());
        context->setPixelShader(assist_ps_.get());
        context->setInputLayout(assist_il_.get());
    }

    void AssistConfigure::close() {
        assist_cb_.reset();
        assist_il_.reset();
        assist_ps_.reset();
        assist_vs_.reset();
    }

    void AssistConfigure::setMatrix(
        GPUContext* context, const utl::mat4f& matrix)
    {
        auto assist_cb = static_cast<AssistConstBuffer*>(
            context->lock(assist_cb_.get(), GPUContext::LOCK_WRITE, nullptr));
        if (assist_cb) {
            assist_cb->wvp = matrix;
            context->unlock(assist_cb_.get());
        }
        context->setVConstantBuffers(0, 1, &assist_cb_);
    }

}
