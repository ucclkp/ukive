// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_GRAPHIC_DEVICE_MANAGER_MAC_H_
#define UKIVE_GRAPHICS_MAC_GRAPHIC_DEVICE_MANAGER_MAC_H_

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {
namespace mac {

    class GraphicDeviceManagerMac : public GraphicDeviceManager {
    public:
        GraphicDeviceManagerMac() = default;

        bool initialize() override;
        void destroy() override;

        GPtr<GPUDevice> getGPUDevice() const override;
        GPtr<GPUContext> getGPUContext() const override;
    };
}
}

#endif  // UKIVE_GRAPHICS_MAC_GRAPHIC_DEVICE_MANAGER_MAC_H_
