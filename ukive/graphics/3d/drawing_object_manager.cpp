// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "drawing_object_manager.h"

#include "utils/stl_utils.h"
#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukv3d {

    DrawingObjectManager::DrawingObjectManager() {
    }

    DrawingObjectManager::~DrawingObjectManager() {
        utl::STLDeleteElements(&drawing_objs_);
    }

    void DrawingObjectManager::add(
        const std::shared_ptr<void>& vertices,
        const std::shared_ptr<int>& indices,
        unsigned int struct_size, unsigned int vertex_count, unsigned int index_count, int tag)
    {
        if (!vertices || !indices || vertex_count == 0 || index_count == 0) {
            LOG(Log::ERR) << "Invalid params.";
            return;
        }

        for (auto obj : drawing_objs_) {
            if (obj->tag == tag) {
                LOG(Log::ERR) << "Invalid params.";
                return;
            }
        }

        DrawingObject* d_object = new DrawingObject();
        d_object->tag = tag;
        d_object->vertices = vertices;
        d_object->vertex_count = vertex_count;
        d_object->vertex_struct_size = struct_size;
        d_object->indices = indices;
        d_object->index_count = index_count;

        createBuffers(d_object);

        drawing_objs_.push_back(d_object);
    }

    DrawingObjectManager::DrawingObject* DrawingObjectManager::getByTag(int tag) {
        for (auto obj : drawing_objs_) {
            if (obj->tag == tag) {
                return obj;
            }
        }
        return nullptr;
    }

    DrawingObjectManager::DrawingObject* DrawingObjectManager::getByPos(size_t pos) {
        size_t index = 0;

        for (auto it = drawing_objs_.begin();
            it != drawing_objs_.end(); ++it, ++index)
        {
            if (pos == index) {
                return *it;
            }
        }

        return nullptr;
    }

    size_t DrawingObjectManager::getCount() {
        return drawing_objs_.size();
    }

    bool DrawingObjectManager::contains(int tag) {
        for (auto obj : drawing_objs_) {
            if (obj->tag == tag) {
                return true;
            }
        }

        return false;
    }

    void DrawingObjectManager::removeByTag(int tag) {
        for (auto it = drawing_objs_.begin();
            it != drawing_objs_.end(); ++it) {
            if ((*it)->tag == tag) {
                drawing_objs_.erase(it);
                return;
            }
        }
    }

    void DrawingObjectManager::removeByPos(size_t pos) {
        size_t index = 0;

        for (auto it = drawing_objs_.begin();
            it != drawing_objs_.end(); ++it, ++index)
        {
            if (pos == index) {
                drawing_objs_.erase(it);
                return;
            }
        }
    }

    void DrawingObjectManager::draw(int tag) {
        auto gpu_context = ukive::Application::getGraphicDeviceManager()->getGPUContext();
        auto object = getByTag(tag);
        if (object) {
            gpu_context->setVertexBuffers(
                0, 1, object->vertex_buffer.get(),
                &object->vertex_struct_size,
                &object->vertex_data_offset);
            gpu_context->setIndexBuffer(object->index_buffer.get(), ukive::GPUDataFormat::R32_UINT, 0);
            gpu_context->drawIndexed(object->index_count, 0, 0);
        }
    }

    void DrawingObjectManager::notifyGraphicDeviceLost() {
        for (auto obj : drawing_objs_) {
            obj->vertex_buffer.reset();
            obj->index_buffer.reset();
        }
    }

    void DrawingObjectManager::notifyGraphicDeviceRestored() {
        for (auto obj : drawing_objs_) {
            createBuffers(obj);
        }
    }

    void DrawingObjectManager::createBuffers(DrawingObject* obj) {
        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        using namespace ukive;
        GPUBuffer::Desc vb;
        vb.is_dynamic = true;
        vb.byte_width = obj->vertex_struct_size * obj->vertex_count;
        vb.res_type = GPUResource::RES_VERTEX_BUFFER;
        vb.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
        vb.struct_byte_stride = 0;

        GPUBuffer::ResourceData vb_data;
        vb_data.sys_mem = obj->vertices.get();
        vb_data.pitch = 0;
        vb_data.slice_pitch = 0;
        obj->vertex_buffer.reset(device->createBuffer(&vb, &vb_data));

        GPUBuffer::Desc ib;
        ib.is_dynamic = true;
        ib.byte_width = sizeof(int) * obj->index_count;
        ib.res_type = GPUResource::RES_INDEX_BUFFER;
        ib.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
        ib.struct_byte_stride = 0;

        GPUBuffer::ResourceData ib_data;
        ib_data.sys_mem = obj->indices.get();
        ib_data.pitch = 0;
        ib_data.slice_pitch = 0;
        obj->index_buffer.reset(device->createBuffer(&ib, &ib_data));
    }

    // DrawingObject
    DrawingObjectManager::DrawingObject::DrawingObject()
        : tag(-1),
          name("object"),
          vertex_count(0),
          vertex_struct_size(0),
          vertex_data_offset(0),
          index_count(0),
          material_index(0) {}

    DrawingObjectManager::DrawingObject::~DrawingObject() {}

}