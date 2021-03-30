// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_DRAWING_OBJECT_MANAGER_H_
#define UKIVE_GRAPHICS_3D_DRAWING_OBJECT_MANAGER_H_

#include <list>
#include <memory>


namespace ukive {
    class GPUBuffer;
}

namespace ukv3d {

    class DrawingObjectManager {
    public:
        struct DrawingObject {
            DrawingObject();
            ~DrawingObject();

            int tag;
            std::string name;
            std::shared_ptr<void> vertices;
            std::shared_ptr<int> indices;
            std::shared_ptr<ukive::GPUBuffer> vertex_buffer;
            std::shared_ptr<ukive::GPUBuffer> index_buffer;
            unsigned int vertex_count;
            unsigned int vertex_struct_size;
            unsigned int vertex_data_offset;
            unsigned int index_count;
            unsigned int material_index;
        };

        DrawingObjectManager();
        ~DrawingObjectManager();

        void add(
            const std::shared_ptr<void>& vertices,
            const std::shared_ptr<int>& indices,
            unsigned int struct_size, unsigned int vertex_count, unsigned int index_count, int tag);
        DrawingObject* getByTag(int tag);
        DrawingObject* getByPos(size_t pos);
        size_t getCount();
        bool contains(int tag);
        void removeByTag(int tag);
        void removeByPos(size_t pos);

        void draw(int tag);

        void notifyGraphicDeviceLost();
        void notifyGraphicDeviceRestored();

    private:
        void createBuffers(DrawingObject* obj);

        std::list<DrawingObject*> drawing_objs_;
    };

}

#endif  // UKIVE_GRAPHICS_3D_DRAWING_OBJECT_MANAGER_H_