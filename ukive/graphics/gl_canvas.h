// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GL_CANVAS_H_
#define UKIVE_GRAPHICS_GL_CANVAS_H_

#include <string>

#include <Windows.h>

#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {

    class Window;

    class GLCanvas {
    public:
        GLCanvas(Window* w, bool hw_acc);
        GLCanvas(HWND w, bool hw_acc);
        GLCanvas(HWND w, int width, int height);
        ~GLCanvas();

        void resize(int width, int height);
        void render();

        void drawLine(const PointF& start, const PointF& end);
        void drawBezier2(const PointF& start, const PointF& mid, const PointF& end);

        void drawRect(const RectF& rect);
        void fillRect(const RectF& rect);

        void drawCircle(float cx, float cy, float radius);
        void fillCircle(float cx, float cy, float radius);

        void drawBitmap(const char* data, size_t width, size_t height);

    private:
        bool create(HDC hdc);

        HDC hdc_ = nullptr;
        HBITMAP os_bitmap_ = nullptr;
        HGLRC gl_rc_ = nullptr;
        HWND window_ = nullptr;

        std::string data_;
        int width_ = 0;
        int height_ = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GL_CANVAS_H_