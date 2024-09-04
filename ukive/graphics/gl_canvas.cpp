// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/gl_canvas.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"

#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "Glu32.lib")

#define GET_HANDLE(w) static_cast<::ukive::win::WindowImplWin*>(w->getImpl())->getHandle()

#define GL_FUNC(type, name) auto name = reinterpret_cast<type>(wglGetProcAddress(#name));


namespace ukive {

    GLCanvas::GLCanvas(Window* w, bool hw_acc)
        : GLCanvas(GET_HANDLE(w), hw_acc) {}

    GLCanvas::GLCanvas(HWND w, bool hw_acc) {
        HDC hdc = ::GetDC(w);
        if (!hdc) {
            LOG(Log::ERR) << "Failed to get dc.";
            return;
        }

        bool ret = create(hdc);
        if (!ret) {
            ::ReleaseDC(w, hdc);
            hdc_ = nullptr;
        } else {
            hdc_ = hdc;
        }
        window_ = w;
    }

    GLCanvas::GLCanvas(HWND w, int width, int height) {
        HDC hdc = GetDC(w);
        if (!hdc) {
            LOG(Log::ERR) << "Failed to get dc.";
            return;
        }

        HDC offscreen_dc = CreateCompatibleDC(hdc);
        ::ReleaseDC(w, hdc);
        if (!offscreen_dc) {
            return;
        }

        HBITMAP offscreen_bmp = CreateCompatibleBitmap(offscreen_dc, width, height);
        if (!offscreen_bmp) {
            return;
        }

        SelectObject(offscreen_dc, offscreen_bmp);

        bool ret = create(offscreen_dc);
        if (!ret) {
            DeleteDC(offscreen_dc);
            DeleteObject(offscreen_bmp);
            hdc_ = nullptr;
        } else {
            hdc_ = offscreen_dc;
            os_bitmap_ = offscreen_bmp;
        }
    }

    GLCanvas::~GLCanvas() {
        if (gl_rc_) {
            wglDeleteContext(gl_rc_);
        }
        if (os_bitmap_) {
            DeleteObject(os_bitmap_);
            DeleteDC(hdc_);
            hdc_ = nullptr;
        }
        if (hdc_) {
            ::ReleaseDC(window_, hdc_);
        }
    }

    bool GLCanvas::create(HDC hdc) {
        PIXELFORMATDESCRIPTOR pfd;
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER | PFD_DRAW_TO_BITMAP;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cRedBits = 0;
        pfd.cRedShift = 0;
        pfd.cGreenBits = 0;
        pfd.cGreenShift = 0;
        pfd.cBlueBits = 0;
        pfd.cBlueShift = 0;
        pfd.cAlphaBits = 8;
        pfd.cAlphaShift = 0;
        pfd.cAccumBits = 0;
        pfd.cAccumRedBits = 0;
        pfd.cAccumGreenBits = 0;
        pfd.cAccumBlueBits = 0;
        pfd.cAccumAlphaBits = 0;
        pfd.cDepthBits = 0;
        pfd.cStencilBits = 0;
        pfd.cAuxBuffers = 0;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.bReserved = 0;
        pfd.dwLayerMask = 0;
        pfd.dwVisibleMask = 0;
        pfd.dwDamageMask = 0;

        int format_index = ::ChoosePixelFormat(hdc, &pfd);
        if (format_index == 0) {
            LOG(Log::ERR) << "Failed to choose pf: " << ::GetLastError();
            return false;
        }

        if (!::SetPixelFormat(hdc, format_index, &pfd)) {
            LOG(Log::ERR) << "Failed to set pf: " << ::GetLastError();
            return false;
        }

        int active_pixel_format = ::GetPixelFormat(hdc);
        if (active_pixel_format == 0) {
            LOG(Log::ERR) << "Failed to get pf: " << ::GetLastError();
            return false;
        }

        if (!::DescribePixelFormat(hdc, active_pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd)) {
            LOG(Log::ERR) << "Failed to describe pf: " << ::GetLastError();
            return false;
        }

        if ((pfd.dwFlags & PFD_SUPPORT_OPENGL) != PFD_SUPPORT_OPENGL) {
            LOG(Log::ERR) << "Current pixel format cannot support OpenGL.";
            return false;
        }

        gl_rc_ = wglCreateContext(hdc);
        if (!gl_rc_) {
            LOG(Log::ERR) << "Failed to create RC: " << ::GetLastError();
            return false;
        }

        if (wglMakeCurrent(hdc, gl_rc_) == FALSE) {
            LOG(Log::ERR) << "Failed to set current RC: " << ::GetLastError();
            return false;
        }

        auto exts = glGetString(GL_EXTENSIONS);
        auto vender = glGetString(GL_VENDOR);
        auto renderer = glGetString(GL_RENDERER);
        auto version = glGetString(GL_VERSION);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(1, 1, 1, 0.5);
        glDisable(GL_DEPTH_TEST);

        glDisable(GL_CULL_FACE);

        /*GLuint fbo;
        GL_FUNC(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
        if (glGenFramebuffers) {
            glGenFramebuffers(1, &fbo);
        }*/

        auto image = Application::getImageLocFactory()->decodeFile(u"E:\\Test\\test.png", ImageOptions());
        if (image.isValid()) {
            auto frame = image.getFrames()[0];
            width_ = frame->getPixelSize().width();
            height_ = frame->getPixelSize().height();

            size_t stride;
            auto data = frame->lockPixels(IAF_READ, &stride);
            data_.assign((const char*)data, stride * height_);
            frame->unlockPixels();
        }

        return true;
    }

    void GLCanvas::resize(int width, int height) {
        if (window_) {
            RECT rect;
            ::GetWindowRect(window_, &rect);

            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        } else if (os_bitmap_) {
            DeleteObject(os_bitmap_);

            HBITMAP offscreen_bmp = CreateCompatibleBitmap(hdc_, width, height);
            if (!offscreen_bmp) {
                return;
            }

            SelectObject(hdc_, offscreen_bmp);

        }
        
        if (width < 1) {
            width = 1;
        }
        if (height < 1) {
            height = 1;
        }

        glViewport(0, 0, width, height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluOrtho2D(0, width, height, 0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void GLCanvas::render() {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.5f, 0.5f, 0);

        // Draw...
        glColor4f(0, 0, 0, 1);

        //drawRect({ 100, 100, 4, 4 });
        //drawLine({ 100.5, 100 }, { 100.5, 1000 });
        //drawCircle(200, 200, 100);

        drawBezier2({ 100, 100 }, { 300, 300 }, { 500, 100 });

        drawBitmap(data_.data(), width_, height_);

        //glFlush();

        ::SwapBuffers(hdc_);
    }

    void GLCanvas::drawLine(const PointF& start, const PointF& end) {
        glBegin(GL_LINES);
        glVertex2d(start.x(), start.y());
        glVertex2d(end.x(), end.y());
        glEnd();
    }

    void GLCanvas::drawBezier2(const PointF& start, const PointF& mid, const PointF& end) {
        double step = 1.0 / ((end - mid).length() + (mid - start).length());

        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        for (double t = 0; t < 1.0; t += step) {
            double _1_t = 1 - t;
            double x = _1_t * _1_t * start.x() + 2 * t * _1_t * mid.x() + t * t*end.x();
            double y = _1_t * _1_t * start.y() + 2 * t * _1_t * mid.y() + t * t*end.y();
            glVertex2d(x, y);
        }
        glEnd();
    }

    void GLCanvas::drawRect(const RectF& rect) {
        glDisable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_LOOP);
        {
            glVertex2d(rect.x(), rect.y());
            glVertex2d(rect.right(), rect.y());
            glVertex2d(rect.right(), rect.bottom());
            glVertex2d(rect.x(), rect.bottom());
        }
        glEnd();
    }

    void GLCanvas::fillRect(const RectF& rect) {
        glDisable(GL_LINE_SMOOTH);
        glBegin(GL_POLYGON);
        {
            glVertex2d(rect.x(), rect.y());
            glVertex2d(rect.right(), rect.y());
            glVertex2d(rect.right(), rect.bottom());
            glVertex2d(rect.x(), rect.bottom());
        }
        glEnd();
    }

    void GLCanvas::drawCircle(float cx, float cy, float radius) {
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_LOOP);
        {
            for (int i = 0; i < 360; i += 6) {
                double rad = (3.14159265359 / 180) * (90 - i);
                double x = radius * std::sin(rad);
                double y = radius * std::cos(rad);
                glVertex2d(cx + x, cy + y);
            }
        }
        glEnd();
    }

    void GLCanvas::fillCircle(float cx, float cy, float radius) {
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_POLYGON);
        {
            for (int i = 0; i < 360; i += 6) {
                double rad = (3.14159265359 / 180) * (90 - i);
                double x = radius * std::sin(rad);
                double y = radius * std::cos(rad);
                glVertex2d(cx + x, cy + y);
            }
        }
        glEnd();

        glBegin(GL_LINE_LOOP);
        {
            for (int i = 0; i < 360; i += 6) {
                double rad = (3.14159265359 / 180) * (90 - i);
                double x = radius * std::sin(rad);
                double y = radius * std::cos(rad);
                glVertex2d(cx + x, cy + y);
            }
        }
        glEnd();
    }

    void GLCanvas::drawBitmap(const char* data, size_t width, size_t height) {
        glTranslatef(-0.5f, -0.5f, 0);
        glColor4f(1, 1, 1, 1);
        glEnable(GL_TEXTURE_2D);

        GLuint bmp_tex;
        glGenTextures(1, &bmp_tex);
        glBindTexture(GL_TEXTURE_2D, bmp_tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // https://www.khronos.org/opengl/wiki/Textures_-_more
        // https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

        auto err = glGetError();
        auto msg = gluErrorString(err);
        //GL_INVALID_ENUM

        glBegin(GL_QUADS);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(100.f, 100.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(100.f + width, 100.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(100.f + width, 100.f + height);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(100.f, 100.f + height);
        glEnd();

        glDeleteTextures(1, &bmp_tex);

        glDisable(GL_TEXTURE_2D);
        glTranslatef(0.5f, 0.5f, 0);
    }

}