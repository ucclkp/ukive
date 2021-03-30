// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_DISPLAY_MAC_H_
#define UKIVE_GRAPHICS_MAC_DISPLAY_MAC_H_

#include "ukive/graphics/display.h"

#import <CoreGraphics/CoreGraphics.h>


namespace ukive {

    class DisplayMac : public Display {
    public:
        DisplayMac();
        ~DisplayMac();

        bool makePrimary() override;
        bool makeFromPoint(const Point &p) override;
        bool makeFromRect(const Rect &r) override;
        bool makeFromWindow(Window *w) override;

        bool isInHDRMode() const override;

        void getName(std::u16string *name) override;
        void getAdapterName(std::u16string *name) override;
        Rect getBounds() const override;
        Rect getWorkArea() const override;
        Rect getPixelBounds() const override;
        Rect getPixelWorkArea() const override;
        void getUserScale(float *sx, float *sy) const override;
        uint32_t getRefreshRate() const override;

    private:
        bool queryDisplayInfo(CGDirectDisplayID display_id);

        bool is_empty_ = true;
        CGDirectDisplayID monitor_;
        CGDisplayModeRef monitor_mode_ = nullptr;
    };

}

#endif  // UKIVE_GRAPHICS_MAC_DISPLAY_MAC_H_
