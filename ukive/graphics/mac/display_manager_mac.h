// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_DISPLAY_MANAGER_MAC_H_
#define UKIVE_GRAPHICS_MAC_DISPLAY_MANAGER_MAC_H_

#include "ukive/graphics/display_manager.h"


namespace ukive {
namespace mac {

    class WindowImplMac;

    class DisplayManagerMac : public DisplayManager {
    public:
        DisplayManagerMac();

        bool initialize() override;
        void destroy() override;

        DisplayPtr fromNull() override;
        DisplayPtr fromPrimary() override;
        DisplayPtr fromPoint(const Point& p) override;
        DisplayPtr fromRect(const Rect& r) override;
        DisplayPtr fromWindow(const Window* w) override;
        DisplayPtr fromWindowImpl(const WindowImplMac* win);

        const DisplayList& getAllDisplays() const override;

    private:
        DisplayPtr null_display_;
        DisplayList list_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_DISPLAY_MANAGER_MAC_H_
