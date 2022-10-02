// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_3D_SPACE_OBJECT_MANAGER_H_
#define UKIVE_GRAPHICS_3D_SPACE_OBJECT_MANAGER_H_

#include <list>
#include <memory>
#include <string>

#include "ukive/graphics/gptr.hpp"


namespace ukive {
    class GPUBuffer;
    class GPUContext;
}

namespace ukv3d {

    struct SpaceObject;

    class SpaceObjectManager {
    public:
        SpaceObjectManager();
        ~SpaceObjectManager();

        void add(
            const std::shared_ptr<void>& vertices,
            const std::shared_ptr<int>& indices,
            unsigned int struct_size, unsigned int vertex_count, unsigned int index_count, int tag);
        void add(SpaceObject* obj);
        SpaceObject* getByTag(int tag) const;
        SpaceObject* getByPos(size_t pos) const;
        size_t getCount() const;
        bool contains(int tag) const;
        void removeByTag(int tag);
        void removeByPos(size_t pos);

        void draw(ukive::GPUContext* c, int tag);

        void notifyGraphicDeviceLost();
        void notifyGraphicDeviceRestored();

    private:
        std::list<SpaceObject*> space_objs_;
    };

}

#endif  // UKIVE_GRAPHICS_3D_SPACE_OBJECT_MANAGER_H_