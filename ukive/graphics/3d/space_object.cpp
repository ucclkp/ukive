// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "space_object.h"

#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_device.h"


namespace ukv3d {

    SpaceObject::SpaceObject(
        const std::shared_ptr<void>& vertices,
        unsigned int struct_size,
        unsigned int vertex_count,
        const std::string& name,
        int tag)
        : tag(tag),
          name(name),
          vertices(vertices),
          vertex_count(vertex_count),
          vertex_struct_size(struct_size),
          vertex_data_offset(0),
          index_count(0),
          material_index(0) {}

    SpaceObject::SpaceObject(
        const std::shared_ptr<void>& vertices,
        const std::shared_ptr<int>& indices,
        unsigned int struct_size,
        unsigned int vertex_count,
        unsigned int index_count,
        const std::string& name,
        int tag)
        : tag(tag),
          name(name),
          vertices(vertices),
          indices(indices),
          vertex_count(vertex_count),
          vertex_struct_size(struct_size),
          vertex_data_offset(0),
          index_count(index_count),
          material_index(0) {}

    SpaceObject::~SpaceObject() {}

    void SpaceObject::createBuffers(ukive::GPUDevice* d) {
        using namespace ukive;
        GPUBuffer::Desc vb;
        vb.is_dynamic = true;
        vb.byte_width = vertex_struct_size * vertex_count;
        vb.res_type = GPUResource::RES_VERTEX_BUFFER;
        vb.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
        vb.struct_byte_stride = 0;

        GPUBuffer::ResourceData vb_data;
        vb_data.sys_mem = vertices.get();
        vb_data.pitch = 0;
        vb_data.slice_pitch = 0;
        vertex_buffer = d->createBuffer(vb, &vb_data);

        if (indices && index_count) {
            GPUBuffer::Desc ib;
            ib.is_dynamic = true;
            ib.byte_width = sizeof(int) * index_count;
            ib.res_type = GPUResource::RES_INDEX_BUFFER;
            ib.cpu_access_flag = GPUResource::CPU_ACCESS_WRITE;
            ib.struct_byte_stride = 0;

            GPUBuffer::ResourceData ib_data;
            ib_data.sys_mem = indices.get();
            ib_data.pitch = 0;
            ib_data.slice_pitch = 0;
            index_buffer = d->createBuffer(ib, &ib_data);
        }
    }

    void SpaceObject::destroyBuffers() {
        vertex_buffer.reset();
        index_buffer.reset();
    }

    void SpaceObject::draw(ukive::GPUContext* c) {
        c->setVertexBuffers(
            0, 1,
            &vertex_buffer,
            &vertex_struct_size,
            &vertex_data_offset);
        c->setIndexBuffer(index_buffer.get(), ukive::GPUDataFormat::R32_UINT, 0);
        c->drawIndexed(index_count, 0, 0);
    }

}