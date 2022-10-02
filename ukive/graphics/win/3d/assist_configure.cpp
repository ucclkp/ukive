// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "assist_configure.h"

#include "ukive/app/application.h"
#include "ukive/graphics/3d/space_object.h"
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

    ukv3d::SpaceObject* AssistConfigure::createWorldAxisObj(int tag, float length) {
        unsigned int vertex_count = 6;
        unsigned int index_count = 6;

        if (length < 10.0f) {
            length = 10.0f;
        }

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position = { -length, 0, 0 };
        vertex_data[0].color = { 1, 0, 0, 1 };
        vertex_data[1].position = { length, 0, 0 };
        vertex_data[1].color = { 1, 0, 0, 1 };

        vertex_data[2].position = { 0, -length, 0 };
        vertex_data[2].color = { 0, 1, 0, 1 };
        vertex_data[3].position = { 0, length, 0 };
        vertex_data[3].color = { 0, 1, 0, 1 };

        vertex_data[4].position = { 0, 0, -length };
        vertex_data[4].color = { 0, 0, 1, 1 };
        vertex_data[5].position = { 0, 0, length };
        vertex_data[5].color = { 0, 0, 1, 1 };

        auto indices = new int[index_count] {
            0, 1, 2, 3, 4, 5
        };

        return new ukv3d::SpaceObject(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData),
            vertex_count, index_count, {}, tag);
    }

    ukv3d::SpaceObject* AssistConfigure::createLineObj(utl::pt3f* point1, utl::pt3f* point2, int tag) {
        unsigned int vertex_count = 2;
        unsigned int index_count = 2;

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position = *point1;
        vertex_data[0].color = { 0.5f, 0, 0, 1 };
        vertex_data[1].position = *point2;
        vertex_data[1].color = { 0.5f, 0, 0, 1 };

        auto indices = new int[index_count] {
            0, 1
        };

        return new ukv3d::SpaceObject(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData),
            vertex_count, index_count, {}, tag);
    }

    ukv3d::SpaceObject* AssistConfigure::createMarkObj(int tag, utl::pt3f* mark, float length) {
        unsigned int vertex_count = 4;
        unsigned int index_count = 4;

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position = { mark->x() - length, 0, mark->z() };
        vertex_data[0].color = { 0.5f, 0, 0, 1 };
        vertex_data[1].position = { mark->x() + length, 0, mark->z() };
        vertex_data[1].color = { 0.5f, 0, 0, 1 };
        vertex_data[2].position = { mark->x(), 0, mark->z() - length };
        vertex_data[2].color = { 0.5f, 0, 0, 1 };
        vertex_data[3].position = { mark->x(), 0, mark->z() + length };
        vertex_data[3].color = { 0.5f, 0, 0, 1 };

        auto indices = new int[index_count] {
            0, 1, 2, 3
        };

        return new ukv3d::SpaceObject(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData),
            vertex_count, index_count, {}, tag);
    }

    ukv3d::SpaceObject* AssistConfigure::createBlockObj(int tag, const utl::pt3f& posCenter, float radius) {
        unsigned int vertex_count = 8;
        unsigned int index_count = 36;

        auto vertex_data = new ukive::AssistVertexData[vertex_count];
        vertex_data[0].position = {
            posCenter.x() - radius, posCenter.y() - radius, posCenter.z() - radius };
        vertex_data[0].color = { 1, 1, 0, 1 };

        vertex_data[1].position = {
            posCenter.x() + radius, posCenter.y() - radius, posCenter.z() - radius };
        vertex_data[1].color = { 1, 1, 0, 1 };

        vertex_data[2].position = {
            posCenter.x() + radius, posCenter.y() - radius, posCenter.z() + radius };
        vertex_data[2].color = { 1, 1, 0, 1 };

        vertex_data[3].position = {
            posCenter.x() - radius, posCenter.y() - radius, posCenter.z() + radius };
        vertex_data[3].color = { 1, 1, 0, 1 };

        vertex_data[4].position = {
            posCenter.x() - radius, posCenter.y() + radius, posCenter.z() + radius };
        vertex_data[4].color = { 1, 1, 0, 1 };

        vertex_data[5].position = {
            posCenter.x() - radius, posCenter.y() + radius, posCenter.z() - radius };
        vertex_data[5].color = { 1, 1, 0, 1 };

        vertex_data[6].position = {
            posCenter.x() + radius, posCenter.y() + radius, posCenter.z() - radius };
        vertex_data[6].color = { 1, 1, 0, 1 };

        vertex_data[7].position = {
            posCenter.x() + radius, posCenter.y() + radius, posCenter.z() + radius };
        vertex_data[7].color = { 1, 1, 0, 1 };

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

        return new ukv3d::SpaceObject(
            std::shared_ptr<void>(vertex_data),
            std::shared_ptr<int>(indices),
            sizeof(ukive::AssistVertexData),
            vertex_count, index_count, {}, tag);
    }

}
