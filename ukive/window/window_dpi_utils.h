// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_DPI_UTILS_H_
#define UKIVE_WINDOW_WINDOW_DPI_UTILS_H_

#include "ukive/graphics/rect.hpp"
#include "ukive/graphics/size.hpp"


namespace ukive {

    extern const int kDefaultDpi;

    class WindowNative;

    void scaleToNative(WindowNative* w, int* val);
    void scaleToNative(WindowNative* w, Size* size);
    void scaleToNative(WindowNative* w, Point* pt);
    void scaleToNative(WindowNative* w, Rect* rect);
    void scaleFromNative(WindowNative* w, int* val);
    void scaleFromNative(WindowNative* w, Size* size);
    void scaleFromNative(WindowNative* w, Point* pt);
    void scaleFromNative(WindowNative* w, Rect* rect);

}

#endif  // UKIVE_WINDOW_WINDOW_DPI_UTILS_H_