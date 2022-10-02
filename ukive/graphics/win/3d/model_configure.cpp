// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "model_configure.h"

#include "ukive/app/application.h"
#include "ukive/graphics/3d/space_object.h"
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
        panel_vs_ = gpu_device->createVertexShader(vs_bc.data(), vs_bc.size());

        std::string ps_bc;
        res_mgr->getFileData(shader_dir / u"model_pixel_shader.cso", &ps_bc);
        panel_ps_ = gpu_device->createPixelShader(ps_bc.data(), ps_bc.size());

        using GILDesc = GPUInputLayout::Desc;
        GILDesc desc[4];
        desc[0] = GILDesc("POSITION", GPUDataFormat::R32G32B32_FLOAT, 0, 0, false);
        desc[1] = GILDesc("COLOR", GPUDataFormat::R32G32B32A32_FLOAT);
        desc[2] = GILDesc("NORMAL", GPUDataFormat::R32G32B32_FLOAT);
        desc[3] = GILDesc("TEXCOORD", GPUDataFormat::R32G32_FLOAT);
        panel_il_ = gpu_device->createInputLayout(desc, 4, vs_bc.data(), vs_bc.size());

        GPUBuffer::Desc buf_desc;
        buf_desc.is_dynamic = true;
        buf_desc.byte_width = sizeof(MatrixConstBuffer);
        buf_desc.res_type = GPUResource::RES_CONSTANT_BUFFER;
        buf_desc.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
        buf_desc.struct_byte_stride = 0;
        panel_cb_ = gpu_device->createBuffer(buf_desc, nullptr);
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
        auto panel_cb = static_cast<MatrixConstBuffer*>(
            context->lock(panel_cb_.get(), GPUContext::LOCK_WRITE, nullptr));
        if (panel_cb) {
            panel_cb->wvp = matrix;
            context->unlock(panel_cb_.get());
        }
        context->setVConstantBuffers(0, 1, &panel_cb_);
    }

    ukv3d::SpaceObject* ModelConfigure::createCubeObj(int tag, float edge_len) {
        unsigned int vertex_count = 8;
        unsigned int index_count = 36;

        float half = edge_len / 2.f;

        auto vertex_data = new ukive::ModelVertexData[vertex_count];
        vertex_data[0].position = { -half, -half, -half };
        vertex_data[0].color = { 1, 0, 0, 1 };
        vertex_data[0].texcoord = { 0, 1 };

        vertex_data[1].position = { half, -half, -half };
        vertex_data[1].color = { 1, 0, 0, 1 };
        vertex_data[1].texcoord = { 1, 1 };

        vertex_data[2].position = { half, -half, half };
        vertex_data[2].color = { 0, 1, 0, 1 };
        vertex_data[2].texcoord = { 1, 0 };

        vertex_data[3].position = { -half, -half, half };
        vertex_data[3].color = { 0, 1, 0, 1 };
        vertex_data[3].texcoord = { 0, 0 };

        vertex_data[4].position = { -half, half, half };
        vertex_data[4].color = { 0, 0, 1, 1 };
        vertex_data[4].texcoord = { 0, 0 };

        vertex_data[5].position = { -half, half, -half };
        vertex_data[5].color = { 0, 0, 1, 1 };
        vertex_data[5].texcoord = { 0, 1 };

        vertex_data[6].position = { half, half, -half };
        vertex_data[6].color = { 0, 0, 1, 1 };
        vertex_data[6].texcoord = { 1, 1 };

        vertex_data[7].position = { half, half, half };
        vertex_data[7].color = { 0, 0, 1, 1 };
        vertex_data[7].texcoord = { 1, 0 };

        auto indices = new int[index_count] {
            0, 5, 1,
                1, 5, 6,
                1, 6, 2,
                2, 6, 7,
                2, 7, 3,
                3, 7, 4,
                3, 4, 0,
                0, 4, 5,
                3, 0, 2,
                2, 0, 1,
                5, 4, 6,
                6, 4, 7
        };

        calculateNormalVector(vertex_data, vertex_count, indices, index_count);

        return new ukv3d::SpaceObject(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::ModelVertexData),
            vertex_count, index_count, {}, tag);
    }

    void ModelConfigure::calculateNormalVector(
        ukive::ModelVertexData* vertices, int vertexCount, int* indices, int indexCount) {

        int triangle_count = indexCount / 3;

        for (int i = 0; i < triangle_count; ++i) {
            int i0 = indices[i * 3 + 0];
            int i1 = indices[i * 3 + 1];
            int i2 = indices[i * 3 + 2];

            auto i0Vec = vertices[i0].position;
            auto e0 = vertices[i1].position - i0Vec;
            auto e1 = vertices[i2].position - i0Vec;
            auto face_normal = e0 ^ e1;

            vertices[i0].normal += face_normal;
            vertices[i1].normal += face_normal;
            vertices[i2].normal += face_normal;
        }

        for (int i = 0; i < vertexCount; ++i) {
            vertices[i].normal.nor();
        }
    }

}
