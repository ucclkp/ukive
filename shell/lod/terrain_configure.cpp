// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "terrain_configure.h"

#include "ukive/app/application.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/gpu/gpu_input_layout.h"
#include "ukive/graphics/gpu/gpu_shader.h"
#include "ukive/resources/resource_manager.h"


namespace shell {

    TerrainConfigure::TerrainConfigure() {}

    TerrainConfigure::~TerrainConfigure() {}

    void TerrainConfigure::init() {
        auto res_mgr = ukive::Application::getResourceManager();
        auto gpu_device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        auto shader_dir = res_mgr->getResRootPath() / u"shaders";

        std::string vs_bc;
        res_mgr->getFileData(shader_dir / u"terrain_vertex_shader.cso", &vs_bc);
        vertex_shader_ = gpu_device->createVertexShader(vs_bc.data(), vs_bc.size());

        std::string ps_bc;
        res_mgr->getFileData(shader_dir / u"terrain_pixel_shader.cso", &ps_bc);
        pixel_shader_ = gpu_device->createPixelShader(ps_bc.data(), ps_bc.size());

        using GILDesc = ukive::GPUInputLayout::Desc;
        GILDesc desc[2];
        desc[0] = GILDesc("POSITION", ukive::GPUDataFormat::R32G32B32_FLOAT, 0, 0, false);
        desc[1] = GILDesc("TEXCOORD", ukive::GPUDataFormat::R32G32_FLOAT);
        input_layout_ = gpu_device->createInputLayout(desc, 2, vs_bc.data(), vs_bc.size());

        ukive::GPUBuffer::Desc buf_desc;
        buf_desc.is_dynamic = true;
        buf_desc.byte_width = sizeof(MatrixConstBuffer);
        buf_desc.res_type = ukive::GPUResource::RES_CONSTANT_BUFFER;
        buf_desc.cpu_access_flag = ukive::GPUResource::CPU_ACCESS_WRITE;
        buf_desc.struct_byte_stride = 0;
        const_buffer_ = gpu_device->createBuffer(buf_desc, nullptr);
    }

    void TerrainConfigure::active(ukive::GPUContext* context) {
        context->setVertexShader(vertex_shader_.get());
        context->setPixelShader(pixel_shader_.get());
        context->setInputLayout(input_layout_.get());
    }

    void TerrainConfigure::close() {
        const_buffer_.reset();
        input_layout_.reset();
        pixel_shader_.reset();
        vertex_shader_.reset();
    }

    void TerrainConfigure::setMatrix(
        ukive::GPUContext* context, const utl::mat4f& matrix)
    {
        auto cb = static_cast<MatrixConstBuffer*>(context->lock(const_buffer_.get()));
        if (cb) {
            cb->wvp = matrix;
            context->unlock(const_buffer_.get());
        }
        context->setVConstantBuffers(0, 1, &const_buffer_);
    }

}
