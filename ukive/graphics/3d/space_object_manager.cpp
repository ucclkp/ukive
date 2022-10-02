// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "space_object_manager.h"

#include "utils/stl_utils.h"
#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/gpu/gpu_buffer.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/3d/space_object.h"


namespace ukv3d {

    SpaceObjectManager::SpaceObjectManager() {
    }

    SpaceObjectManager::~SpaceObjectManager() {
        utl::STLDeleteElements(&space_objs_);
    }

    void SpaceObjectManager::add(
        const std::shared_ptr<void>& vertices,
        const std::shared_ptr<int>& indices,
        unsigned int struct_size,
        unsigned int vertex_count,
        unsigned int index_count,
        int tag)
    {
        if (!vertices || !indices || vertex_count == 0 || index_count == 0) {
            LOG(Log::ERR) << "Invalid params.";
            return;
        }

        for (auto obj : space_objs_) {
            if (obj->tag == tag) {
                LOG(Log::ERR) << "Invalid params.";
                return;
            }
        }

        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();

        SpaceObject* d_object = new SpaceObject(
            vertices, indices, struct_size, vertex_count, index_count, {}, tag);
        d_object->createBuffers(device.get());
        space_objs_.push_back(d_object);
    }

    void SpaceObjectManager::add(SpaceObject* obj) {
        for (auto o : space_objs_) {
            if (o == obj) {
                return;
            }
        }

        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();
        obj->createBuffers(device.get());
        space_objs_.push_back(obj);
    }

    SpaceObject* SpaceObjectManager::getByTag(int tag) const {
        for (auto obj : space_objs_) {
            if (obj->tag == tag) {
                return obj;
            }
        }
        return nullptr;
    }

    SpaceObject* SpaceObjectManager::getByPos(size_t pos) const {
        size_t index = 0;

        for (auto it = space_objs_.begin();
            it != space_objs_.end(); ++it, ++index)
        {
            if (pos == index) {
                return *it;
            }
        }

        return nullptr;
    }

    size_t SpaceObjectManager::getCount() const {
        return space_objs_.size();
    }

    bool SpaceObjectManager::contains(int tag) const {
        for (auto obj : space_objs_) {
            if (obj->tag == tag) {
                return true;
            }
        }

        return false;
    }

    void SpaceObjectManager::removeByTag(int tag) {
        for (auto it = space_objs_.begin();
            it != space_objs_.end(); ++it) {
            if ((*it)->tag == tag) {
                space_objs_.erase(it);
                return;
            }
        }
    }

    void SpaceObjectManager::removeByPos(size_t pos) {
        size_t index = 0;

        for (auto it = space_objs_.begin();
            it != space_objs_.end(); ++it, ++index)
        {
            if (pos == index) {
                space_objs_.erase(it);
                return;
            }
        }
    }

    void SpaceObjectManager::draw(ukive::GPUContext* c, int tag) {
        auto object = getByTag(tag);
        if (object) {
            object->draw(c);
        }
    }

    void SpaceObjectManager::notifyGraphicDeviceLost() {
        for (auto obj : space_objs_) {
            obj->destroyBuffers();
        }
    }

    void SpaceObjectManager::notifyGraphicDeviceRestored() {
        auto device = ukive::Application::getGraphicDeviceManager()->getGPUDevice();
        for (auto obj : space_objs_) {
            obj->createBuffers(device.get());
        }
    }

}