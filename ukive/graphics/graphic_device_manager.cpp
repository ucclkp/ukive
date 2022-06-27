// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "graphic_device_manager.h"

#include "utils/multi_callbacks.hpp"
#include "utils/platform_utils.h"

#include "ukive/graphics/graphic_context_change_listener.h"
#include "ukive/graphics/rebuildable.h"

#ifdef OS_WINDOWS
#include "ukive/graphics/win/directx_manager.h"
#elif defined OS_MAC
#include "ukive/graphics/mac/graphic_device_manager_mac.h"
#endif


namespace ukive {

    // static
    GraphicDeviceManager* GraphicDeviceManager::create() {
#ifdef OS_WINDOWS
        return new win::DirectXManager();
#elif defined OS_MAC
        return new mac::GraphicDeviceManagerMac();
#endif
    }

    GraphicDeviceManager::GraphicDeviceManager() {}

    void GraphicDeviceManager::addListener(GraphicContextChangeListener* l) {
        utl::addCallbackTo(listeners_, l);
    }

    void GraphicDeviceManager::removeListener(GraphicContextChangeListener* l) {
        utl::removeCallbackFrom(listeners_, l);
    }

    void GraphicDeviceManager::notifyDeviceLost() {
        for (auto listener : listeners_) {
            listener->onGraphDeviceLost();
        }
    }

    void GraphicDeviceManager::notifyDeviceRestored() {
        for (auto listener : listeners_) {
            listener->onGraphDeviceRestored();
        }
    }

    void GraphicDeviceManager::demolishRbs() {
        std::lock_guard<std::recursive_mutex> lg(rb_sync_);
        auto rb = rb_head_;
        while (rb) {
            rb->demolish();
            rb = rb->getRebuildNext();
        }
    }

    void GraphicDeviceManager::rebuildRbs(bool succeeded) {
        std::lock_guard<std::recursive_mutex> lg(rb_sync_);
        auto rb = rb_head_;
        while (rb) {
            rb->rebuild(succeeded);
            rb = rb->getRebuildNext();
        }
    }

    void GraphicDeviceManager::notifyHDRChanged(bool hdr) {
        for (auto listener : listeners_) {
            listener->onHDRChanged(hdr);
        }
    }

    void GraphicDeviceManager::notifyDPIChanged(float dpi_x, float dpi_y) {
        for (auto listener : listeners_) {
            listener->onDPIChanged(dpi_x, dpi_y);
        }
    }

}
