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
        GLCanvas(int width, int height);
        GLCanvas(Window* w, bool hw_acc);
        ~GLCanvas();

        void resize();
        void render();

        void drawLine(const PointF& start, const PointF& end);
        void drawBezier2(const PointF& start, const PointF& mid, const PointF& end);

        void drawRect(const RectF& rect);
        void fillRect(const RectF& rect);

        void drawCircle(float cx, float cy, float radius);
        void fillCircle(float cx, float cy, float radius);

        void drawBitmap(const char* data, size_t width, size_t height);

    private:
        HDC hdc_ = nullptr;
        HGLRC gl_rc_ = nullptr;
        Window* window_ = nullptr;

        std::string data_;
        int width_, height_;
    };

}

#endif  // UKIVE_GRAPHICS_GL_CANVAS_H_