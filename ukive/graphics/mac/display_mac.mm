// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "display_mac.h"

#include "ukive/window/window.h"
#include "ukive/window/mac/window_impl_mac.h"

#import <Cocoa/Cocoa.h>
#import "ukive/window/mac/uk_ns_window.h"


namespace ukive {

    DisplayMac::DisplayMac() {
    }

    DisplayMac::~DisplayMac() {
        if (!is_empty_ && monitor_mode_) {
            CGDisplayModeRelease(monitor_mode_);
        }
    }

    bool DisplayMac::makePrimary() {
        return queryDisplayInfo(CGMainDisplayID());
    }

    bool DisplayMac::makeFromPoint(const Point &p) {
        uint32_t actual_count = 0;
        CGDirectDisplayID display;
        auto ret = CGGetDisplaysWithPoint(
                               CGPointMake(p.x, p.y),
                               1, &display, &actual_count);
        if (ret != kCGErrorSuccess || actual_count != 1) {
            return false;
        }

        return queryDisplayInfo(display);
    }

    bool DisplayMac::makeFromRect(const Rect &r) {
        uint32_t actual_count = 0;
        CGDirectDisplayID display;
        auto ret = CGGetDisplaysWithRect(
                                         CGRectMake(r.left, r.top, r.width(), r.height()),
                                         1, &display, &actual_count);
        if (ret != kCGErrorSuccess || actual_count != 1) {
            return false;
        }

        return queryDisplayInfo(display);
    }

    bool DisplayMac::makeFromWindow(Window *w) {
        if (!w) {
            return false;
        }

        auto win = static_cast<WindowImplMac*>(w->getImpl());
        if (!win) {
            return false;
        }

        if (win->isCreated()) {
            auto ns_win = win->getNSWindow();
            auto dict = [[ns_win screen] deviceDescription];
            NSNumber* screen_id = [dict objectForKey:@"NSScreenNumber"];
            return queryDisplayInfo(screen_id.unsignedIntValue);
        }

        auto bounds = w->getBounds();
        return makeFromRect(bounds);
    }

    bool DisplayMac::isInHDRMode() const {
        return false;
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
