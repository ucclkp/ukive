// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_GL_CANVAS_H_
#define UKIVE_GRAPHICS_GL_CANVAS_H_

#include <string>
#include <vector>

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

        bool copy(void* pixels);

        HDC getHDC() const { return hdc_; }

    private:
        struct Image {
            int width = 0;
            int height = 0;
            std::vector<char> data;
        };

        bool createHWNDRenderTarget(HDC hdc);
        bool createOffscreenRenderTarget(HWND w, int width, int height);
        bool prepareResources();

        HDC hdc_ = nullptr;
        HGLRC gl_rc_ = nullptr;
        HWND window_ = nullptr;

        Image img_;
        int rt_width_ = 0;
        int rt_height_ = 0;
    };

}

#endif  // UKIVE_GRAPHICS_GL_CANVAS_H_