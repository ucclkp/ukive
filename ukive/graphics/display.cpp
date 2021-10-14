// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/display.h"

#include "ukive/app/application.h"
#include "ukive/graphics/display_manager.h"


namespace ukive {

    // static
    Display::DisplayPtr Display::fromNull() {
        return Application::getDisplayManager()->fromNull();
    }

    // static
    Display::DisplayPtr Display::fromPrimary() {
        return Application::getDisplayManager()->fromPrimary();
    }

    // static
    Display::DisplayPtr Display::fromPoint(const Point& p) {
        return Application::getDisplayManager()->fromPoint(p);
    }

    // static
    Display::DisplayPtr Display::fromRect(const Rect& r) {
        return Application::getDisplayManager()->fromRect(r);
    }

    // static
    Display::DisplayPtr Display::fromWindow(Window* w) {
        return Application::getDisplayManager()->fromWindow(w);
    }

}
