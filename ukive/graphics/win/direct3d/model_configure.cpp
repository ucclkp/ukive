// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "model_configure.h"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gpu/gpu_input_layout.h"
#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/resources/resource_manager.h"


namespace ukive {

    ModelConfigure::ModelConfigure() {}

    ModelConfigure::~ModelConfigure() {}

    void ModelConfigure::init() {
        auto res_mgr = Application::getResourceManager();
        auto gpu_device = Application::getGraphicDeviceManager()->getGPUDevice();

        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string vs_bc;
        res_mgr->getFileData(shader_dir / u"model_vertex_shader.cso", &vs_bc);
        panel_vs_.reset(gpu_device->createVertexShader(vs_bc.data(), vs_bc.size()));

        std::string ps_bc;
        res_mgr->getFileData(shader_dir / u"model_pixel_shader.cso", &ps_bc);
        panel_ps_.reset(gpu_device->createPixelShader(ps_bc.data(), ps_bc.size()));

        using GILDesc = GPUInputLayout::Desc;
        GILDesc desc[4];
        desc[0] = GILDesc("POSITION", 0, 0, 0, GPUDataFormat::R32G32B32_FLOAT);
        desc[1] = GILDesc("COLOR", 0, 0, GPUInputLayout::Append, GPUDataFormat::R32G32B32A32_FLOAT);
        desc[2] = GILDesc("NORMAL", 0, 0, GPUInputLayout::Append, GPUDataFormat::R32G32B32_FLOAT);
        desc[3] = GILDesc("TEXCOORD", 0, 0, GPUInputLayout::Append, GPUDataFormat::R32G32_FLOAT);
        panel_il_.reset(gpu_device->createInputLayout(desc, 4, vs_bc.data(), vs_bc.size()));

        GPUBuffer::Desc buf_desc;
        buf_desc.is_dynamic = true;
        buf_desc.byte_width = sizeof(MatrixConstBuffer);
        buf_desc.res_type = GPUResource::RES_CONSTANT_BUFFER;
        buf_desc.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
        buf_desc.struct_byte_stride = 0;
        panel_cb_.reset(gpu_device->createBuffer(&buf_desc, nullptr));
    }

    void ModelConfigure::active(GPUContext* context) {
        context->setVertexShader(panel_vs_.get());
        context->setPixelShader(panel_ps_.get());
        context->setInputLayout(panel_il_.get());
    }

    void ModelConfigure::close() {
        panel_il_.reset();
        panel_ps_.reset();
        panel_vs_.reset();
        panel_cb_.reset();
    }

    void ModelConfigure::setMatrix(
        GPUContext* context, const utl::mat4f& matrix)
    {
        auto panel_cb = static_cast<MatrixConstBuffer*>(context->lock(panel_cb_.get()));
        if (panel_cb) {
            panel_cb->wvp = matrix;
            context->unlock(panel_cb_.get());
        }
        context->setVConstantBuffers(0, 1, panel_cb_.get());
    }

}
