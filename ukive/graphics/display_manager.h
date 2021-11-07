// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_DISPLAY_MANAGER_H_
#define UKIVE_GRAPHICS_DISPLAY_MANAGER_H_

#include <memory>
#include <vector>

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Display;
    class Window;

    class DisplayStatusListener {
    public:
        virtual ~DisplayStatusListener() = default;

        virtual void onDisplayChanged() = 0;
    };


    class DisplayManager {
    public:
        using DisplayPtr = std::shared_ptr<Display>;
        using DisplayList = std::vector<DisplayPtr>;

        static DisplayManager* create();

        virtual ~DisplayManager() = default;

        virtual bool initialize() = 0;
        virtual void destroy() = 0;

        virtual DisplayPtr fromNull() = 0;
        virtual DisplayPtr fromPrimary() = 0;
        virtual DisplayPtr fromPoint(const Point& p) = 0;
        virtual DisplayPtr fromRect(const Rect& r) = 0;
        virtual DisplayPtr fromWindow(const Window* w) = 0;

        virtual const DisplayList& getAllDisplays() const = 0;

        void addListener(DisplayStatusListener* l);
        void removeListener(DisplayStatusListener* l);

    protected:
        std::vector<DisplayStatusListener*> listeners_;
    };

}

#endif  // UKIVE_GRAPHICS_DISPLAY_MANAGER_H_