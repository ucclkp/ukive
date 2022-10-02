// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_SPACE_OBJECT_H_
#define UKIVE_GRAPHICS_3D_SPACE_OBJECT_H_

#include <memory>
#include <string>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/gpu/gpu_buffer.h"


namespace ukive {
    class GPUContext;
    class GPUDevice;
}

namespace ukv3d {

    struct SpaceObject {
        SpaceObject(
            const std::shared_ptr<void>& vertices,
            unsigned int struct_size,
            unsigned int vertex_count,
            const std::string& name,
            int tag = -1);
        SpaceObject(
            const std::shared_ptr<void>& vertices,
            const std::shared_ptr<int>& indices,
            unsigned int struct_size,
            unsigned int vertex_count,
            unsigned int index_count,
            const std::string& name,
            int tag = -1);
        ~SpaceObject();

        void createBuffers(ukive::GPUDevice* d);
        void destroyBuffers();

        void draw(ukive::GPUContext* c);

        int tag;
        std::string name;
        std::shared_ptr<void> vertices;
        std::shared_ptr<int> indices;
        ukive::GPtr<ukive::GPUBuffer> vertex_buffer;
        ukive::GPtr<ukive::GPUBuffer> index_buffer;
        unsigned int vertex_count;
        unsigned int vertex_struct_size;
        unsigned int vertex_data_offset;
        unsigned int index_count;
        unsigned int material_index;
    };

}

#endif  // UKIVE_GRAPHICS_3D_SPACE_OBJECT_H_