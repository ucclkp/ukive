// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHIC_GRAPHIC_DEVICE_MANAGER_H_
#define UKIVE_GRAPHIC_GRAPHIC_DEVICE_MANAGER_H_

#include <vector>

#include "ukive/graphics/gpu/gpu_context.h"
#include "ukive/graphics/gpu/gpu_device.h"


namespace ukive {

    class GraphicContextChangeListener;

    class GraphicDeviceManager {
    public:
        static GraphicDeviceManager* create();

        virtual ~GraphicDeviceManager() = default;

        virtual bool initialize() = 0;
        virtual bool recreate() = 0;
        virtual void destroy() = 0;

        virtual GPUDevice* getGPUDevice() const = 0;
        virtual GPUContext* getGPUContext() const = 0;

        void addListener(GraphicContextChangeListener* l);
        void removeListener(GraphicContextChangeListener* l);

        void notifyDeviceLost();
        void notifyDeviceRestored();
        void notifyHDRChanged(bool hdr);
        void notifyDPIChanged(float dpi_x, float dpi_y);

    private:
        std::vector<GraphicContextChangeListener*> listeners_;
    };

}

#endif  // UKIVE_GRAPHIC_GRAPHIC_DEVICE_MANAGER_H_