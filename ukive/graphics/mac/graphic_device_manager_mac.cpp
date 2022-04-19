// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/graphic_device_manager_mac.h"


namespace ukive {
namespace mac {

    bool GraphicDeviceManagerMac::initialize() {
        return true;
    }

    bool GraphicDeviceManagerMac::recreate() {
        return true;
    }

    void GraphicDeviceManagerMac::destroy() {

    }

    GPtr<GPUDevice> GraphicDeviceManagerMac::getGPUDevice() const {
        return {};
    }

    GPtr<GPUContext> GraphicDeviceManagerMac::getGPUContext() const {
        return {};
    }

}
}
