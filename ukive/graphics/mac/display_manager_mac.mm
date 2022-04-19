// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "display_manager_mac.h"

#include "utils/log.h"

#include "ukive/graphics/mac/display_mac.h"
#include "ukive/window/window.h"
#include "ukive/window/mac/window_impl_mac.h"

#import <Cocoa/Cocoa.h>
#import "ukive/window/mac/uk_ns_window.h"


namespace ukive {
namespace mac {

    DisplayManagerMac::DisplayManagerMac() {}

    bool DisplayManagerMac::initialize() {
        null_display_ = std::make_shared<DisplayMac>(false, 0);
        return true;
    }

    void DisplayManagerMac::destroy() {
        list_.clear();
        null_display_.reset();
    }

    DisplayManager::DisplayPtr DisplayManagerMac::fromNull() {
        return null_display_;
    }

    DisplayManager::DisplayPtr DisplayManagerMac::fromPrimary() {
        CGDirectDisplayID id = CGMainDisplayID();
        auto ptr = std::make_shared<DisplayMac>(true, id);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerMac::fromPoint(const Point& p) {
        uint32_t actual_count = 0;
        CGDirectDisplayID display;
        auto ret = CGGetDisplaysWithPoint(
                        CGPointMake(p.x(), p.y()),
                        1, &display, &actual_count);
        if (ret != kCGErrorSuccess || actual_count != 1) {
            return {};
        }

        auto ptr = std::make_shared<DisplayMac>(true, display);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerMac::fromRect(const Rect& r) {
        uint32_t actual_count = 0;
        CGDirectDisplayID display;
        auto ret = CGGetDisplaysWithRect(
                        CGRectMake(r.left, r.top, r.width(), r.height()),
                        1, &display, &actual_count);
        if (ret != kCGErrorSuccess || actual_count != 1) {
            return {};
        }

        auto ptr = std::make_shared<DisplayMac>(true, display);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerMac::fromWindow(const Window* w) {
        if (!w) {
            return {};
        }

        auto win = static_cast<WindowImplMac*>(w->getImpl());
        if (!win) {
            return {};
        }

        return fromWindowImpl(win);
    }

    DisplayManager::DisplayPtr DisplayManagerMac::fromWindowImpl(const WindowImplMac* win) {
        if (win->isCreated()) {
            auto ns_win = win->getNSWindow();
            auto dict = [[ns_win screen] deviceDescription];
            NSNumber* screen_id = [dict objectForKey:@"NSScreenNumber"];

            auto ptr = std::make_shared<DisplayMac>(true, screen_id.unsignedIntValue);
            list_.push_back(ptr);
            return ptr;
        }

        auto bounds = win->getBounds();
        return fromRect(bounds);
    }

    const DisplayManager::DisplayList& DisplayManagerMac::getAllDisplays() const {
        return list_;
    }

}
}
