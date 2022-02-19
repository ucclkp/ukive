// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "display_mac.h"

#include "ukive/app/application.h"
#include "ukive/window/window.h"
#include "ukive/window/mac/window_impl_mac.h"
#include "ukive/graphics/mac/display_manager_mac.h"


namespace ukive {
namespace mac {

    // static
    Display::DisplayPtr DisplayMac::fromWindowImpl(const WindowImplMac* win) {
        return static_cast<mac::DisplayManagerMac*>(
            Application::getDisplayManager())->fromWindowImpl(win);
    }

    DisplayMac::DisplayMac(bool valid, CGDirectDisplayID display_id)
        : monitor_(display_id)
    {
        if (valid) {
            queryDisplayInfo(display_id);
        }
    }

    DisplayMac::~DisplayMac() {
        if (!is_empty_ && monitor_mode_) {
            CGDisplayModeRelease(monitor_mode_);
        }
    }

    bool DisplayMac::isValid() const {
        return !is_empty_;
    }

    bool DisplayMac::isInHDRMode() const {
        return false;
    }

    bool DisplayMac::isSame(const Display* rhs) const {
        if (!isValid() || !rhs->isValid()) {
            return !isValid() && !rhs->isValid();
        }

        auto rhs_mac = static_cast<const DisplayMac*>(rhs);
        if (monitor_ != rhs_mac->monitor_) {
            return false;
        }

        if (CGDisplayModeGetPixelWidth(monitor_mode_) !=
                CGDisplayModeGetPixelWidth(rhs_mac->monitor_mode_) ||
            CGDisplayModeGetPixelHeight(monitor_mode_) !=
                CGDisplayModeGetPixelHeight(rhs_mac->monitor_mode_) ||
            CGDisplayModeGetIOFlags(monitor_mode_) !=
                CGDisplayModeGetIOFlags(rhs_mac->monitor_mode_) ||
            CGDisplayModeGetRefreshRate(monitor_mode_) !=
                CGDisplayModeGetRefreshRate(rhs_mac->monitor_mode_))
        {
            return false;
        }

        return true;
    }

    bool DisplayMac::isSameDisplay(const Display* rhs) const {
        if (!isValid() || !rhs->isValid()) {
            return !isValid() && !rhs->isValid();
        }

        auto rhs_mac = static_cast<const DisplayMac*>(rhs);
        if (monitor_ != rhs_mac->monitor_) {
            return false;
        }
        return true;
    }

    void DisplayMac::getName(std::u16string *name) {
        if (is_empty_) {
            name->clear();
            return;
        }
    }

    void DisplayMac::getAdapterName(std::u16string *name) {
        if (is_empty_) {
            name->clear();
            return;
        }
    }

    Rect DisplayMac::getBounds() const {
        if (is_empty_) {
            return {};
        }

        auto cg_rect = CGDisplayBounds(monitor_);
        return Rect(
                    cg_rect.origin.x, cg_rect.origin.y,
                    cg_rect.size.width, cg_rect.size.height);
    }

    Rect DisplayMac::getWorkArea() const {
        return getBounds();
    }

    Rect DisplayMac::getPixelBounds() const {
        return getBounds();
    }

    Rect DisplayMac::getPixelWorkArea() const {
        return getWorkArea();
    }

    void DisplayMac::getUserScale(float *sx, float *sy) const {
        *sx = 1;
        *sy = 1;
    }

    uint32_t DisplayMac::getRefreshRate() const {
        if (is_empty_) {
            return 60;
        }
        return uint32_t(CGDisplayModeGetRefreshRate(monitor_mode_));
    }

    bool DisplayMac::queryDisplayInfo(CGDirectDisplayID display_id) {
        auto display_mode = CGDisplayCopyDisplayMode(display_id);
        if (!display_mode) {
            return false;
        }

        if (!is_empty_ && monitor_mode_) {
            CGDisplayModeRelease(monitor_mode_);
        }

        monitor_ = display_id;
        monitor_mode_ = display_mode;
        is_empty_ = false;
        return true;
    }

}
}
