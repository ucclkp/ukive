// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_DISPLAY_H_
#define UKIVE_GRAPHICS_DISPLAY_H_

#include <cstdint>
#include <memory>
#include <string>

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Window;

    class Display {
    public:
        using DisplayPtr = std::shared_ptr<Display>;

        static DisplayPtr fromNull();
        static DisplayPtr fromPrimary();
        static DisplayPtr fromPoint(const Point& p);
        static DisplayPtr fromRect(const Rect& r);
        static DisplayPtr fromWindow(Window* w);

        virtual ~Display() = default;

        virtual bool isValid() const = 0;
        virtual bool isInHDRMode() const = 0;
        virtual bool isSame(const Display* rhs) const = 0;
        virtual bool isSameDisplay(const Display* rhs) const = 0;

        virtual void getName(std::u16string* name) = 0;
        virtual void getAdapterName(std::u16string* name) = 0;
        virtual Rect getBounds() const = 0;
        virtual Rect getWorkArea() const = 0;
        virtual Rect getPixelBounds() const = 0;
        virtual Rect getPixelWorkArea() const = 0;
        virtual void getUserScale(float* sx, float* sy) const = 0;
        virtual uint32_t getRefreshRate() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_DISPLAY_H_