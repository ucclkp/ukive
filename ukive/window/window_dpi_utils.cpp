// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "window_dpi_utils.h"

#include "utils/platform_utils.h"

#include "ukive/app/application.h"
#include "ukive/window/window_native.h"


namespace ukive {

#ifdef OS_WINDOWS
#include <WinUser.h>
    const int kDefaultDpi = USER_DEFAULT_SCREEN_DPI;
#elif defined OS_MAC
    const int kDefaultDpi = 72;
#else
    const int kDefaultDpi = 96;
#endif


    void scaleToNative(WindowNative* w, int* val) {
        if (Application::getOptions().is_auto_dpi_scale) {
            *val = int(w->scaleToNative(float(*val)));
        }
    }

    void scaleToNative(WindowNative* w, Size* size) {
        if (Application::getOptions().is_auto_dpi_scale) {
            auto width = w->scaleToNative(float(size->width()));
            auto height = w->scaleToNative(float(size->height()));
            size->width(int(std::ceil(width)));
            size->height(int(std::ceil(height)));
        }
    }

    void scaleToNative(WindowNative* w, Point* pt) {
        if (Application::getOptions().is_auto_dpi_scale) {
            pt->x(int(w->scaleToNative(float(pt->x()))));
            pt->y(int(w->scaleToNative(float(pt->y()))));
        }
    }

    void scaleToNative(WindowNative* w, Rect* rect) {
        if (Application::getOptions().is_auto_dpi_scale) {
            auto x = w->scaleToNative(float(rect->x()));
            auto y = w->scaleToNative(float(rect->y()));
            auto width = w->scaleToNative(float(rect->width()));
            auto height = w->scaleToNative(float(rect->height()));

            rect->xywh(
                int(std::floor(x)),
                int(std::floor(y)),
                int(std::ceil(width)),
                int(std::ceil(height)));
        }
    }

    void scaleFromNative(WindowNative* w, int* val) {
        if (Application::getOptions().is_auto_dpi_scale) {
            *val = int(w->scaleFromNative(float(*val)));
        }
    }

    void scaleFromNative(WindowNative* w, Size* size) {
        if (Application::getOptions().is_auto_dpi_scale) {
            auto width = w->scaleFromNative(float(size->width()));
            auto height = w->scaleFromNative(float(size->height()));
            size->width(int(std::ceil(width)));
            size->height(int(std::ceil(height)));
        }
    }

    void scaleFromNative(WindowNative* w, Point* pt) {
        if (Application::getOptions().is_auto_dpi_scale) {
            pt->x(int(w->scaleFromNative(float(pt->x()))));
            pt->y(int(w->scaleFromNative(float(pt->y()))));
        }
    }

    void scaleFromNative(WindowNative* w, Rect* rect) {
        if (Application::getOptions().is_auto_dpi_scale) {
            auto x = w->scaleFromNative(float(rect->x()));
            auto y = w->scaleFromNative(float(rect->y()));
            auto width = w->scaleFromNative(float(rect->width()));
            auto height = w->scaleFromNative(float(rect->height()));

            rect->xywh(
                int(std::floor(x)),
                int(std::floor(y)),
                int(std::ceil(width)),
                int(std::ceil(height)));
        }
    }


}
