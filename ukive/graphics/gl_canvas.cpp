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


typedef BOOL(WINAPI* PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
typedef BOOL(WINAPI* PFNWGLQUERYPBUFFERARBPROC) (HPBUFFERARB hPbuffer, int iAttribute, int* piValue);
typedef HDC(WINAPI* PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef HPBUFFERARB(WINAPI* PFNWGLCREATEPBUFFERARBPROC) (HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int* piAttribList);
typedef int (WINAPI* PFNWGLRELEASEPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer, HDC hDC);

PFNWGLDESTROYPBUFFERARBPROC                       wglDestroyPbufferARB;
PFNWGLQUERYPBUFFERARBPROC                         wglQueryPbufferARB;
PFNWGLGETPBUFFERDCARBPROC                         wglGetPbufferDCARB;
PFNWGLCREATEPBUFFERARBPROC                        wglCreatePbufferARB;
PFNWGLRELEASEPBUFFERDCARBPROC                     wglReleasePbufferDCARB;

typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
typedef BOOL(WINAPI* PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, FLOAT* pfValues);
typedef BOOL(WINAPI* PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, int* piValues);

PFNWGLCHOOSEPIXELFORMATARBPROC                    wglChoosePixelFormatARB;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC               wglGetPixelFormatAttribfvARB;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC               wglGetPixelFormatAttribivARB;


bool InitGLExtensions()
{
#define GPA(x) wglGetProcAddress(x)

    // WGL_ARB_pbuffer.
    wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)GPA("wglDestroyPbufferARB");
    wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)GPA("wglQueryPbufferARB");
    wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)GPA("wglGetPbufferDCARB");
    wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)GPA("wglCreatePbufferARB");
    wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)GPA("wglReleasePbufferDCARB");

    // WGL_ARB_pixel_format.
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)GPA("wglChoosePixelFormatARB");
    wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)GPA("wglGetPixelFormatAttribfvARB");
    wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)GPA("wglGetPixelFormatAttribivARB");

#undef GPA

    if (!wglDestroyPbufferARB || !wglQueryPbufferARB || !wglGetPbufferDCARB || !wglCreatePbufferARB || !wglReleasePbufferDCARB)
    {
        return false;
    }

    if (!wglChoosePixelFormatARB || !wglGetPixelFormatAttribfvARB || !wglGetPixelFormatAttribivARB)
    {
        return false;
    }

    return true;
}

HDC   g_hPBufferDC;
HGLRC g_hPBufferRC;
HPBUFFERARB g_hPBuffer;

bool InitPBuffer(HDC hdc, int w, int h)
{
    // Create a pbuffer for off-screen rendering. Notice that since we aren't
    // going to be using the pbuffer for dynamic texturing (i.e., using the
    // pbuffer containing our rendered scene as a texture) we don't need to
    // request for WGL_BIND_TO_TEXTURE_RGBA_ARB support in the attribute list.

    int attribList[] =
    {
        WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // allow rendering to the pbuffer
        WGL_SUPPORT_OPENGL_ARB,  TRUE,      // associate with OpenGL
        WGL_DOUBLE_BUFFER_ARB,   FALSE,     // single buffered
        WGL_RED_BITS_ARB,   8,              // minimum 8-bits for red channel
        WGL_GREEN_BITS_ARB, 8,              // minimum 8-bits for green channel
        WGL_BLUE_BITS_ARB, 8,              // minimum 8-bits for blue channel
        WGL_ALPHA_BITS_ARB, 8,              // minimum 8-bits for alpha channel
        WGL_DEPTH_BITS_ARB, 16,             // minimum 16-bits for depth buffer
        0
    };

    int format = 0;
    UINT matchingFormats = 0;

    if (!wglChoosePixelFormatARB(hdc, attribList, 0, 1, &format, &matchingFormats))
    {
        return false;
    }

    if (!(g_hPBuffer = wglCreatePbufferARB(hdc, format, w, h, 0)))
    {
        return false;
    }

    if (!(g_hPBufferDC = wglGetPbufferDCARB(g_hPBuffer)))
    {
        return false;
    }

    if (!(g_hPBufferRC = wglCreateContext(g_hPBufferDC)))
    {
        return false;
    }

    return true;
}

void CopyPBufferToImage(BYTE* dst, int stride, int w, int h)
{
    // Copy the contents of the framebuffer - which in our case is our pbuffer -
    // to our bitmap image in local system memory. Notice that we also need
    // to invert the pbuffer's pixel data since OpenGL by default orients the
    // bitmap image bottom up. Our Windows DIB wrapper expects images to be
    // top down in orientation.

    BYTE* pixels = new BYTE[w * h * 4];

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

    for (int i = 0; i < h; ++i)
    {
        memcpy(
            &dst[stride * i],
            &pixels[((h - 1) - i) * (w * 4)], w * 4);
    }

    delete[] pixels;
}



namespace ukive {

    GLCanvas::GLCanvas(Window* w, bool hw_acc)
        : GLCanvas(GET_HANDLE(w), hw_acc) {}

    GLCanvas::GLCanvas(HWND w, bool hw_acc) {
        HDC hdc = ::GetDC(w);
        if (!hdc) {
            LOG(Log::ERR) << "Failed to get dc.";
            return;
        }

        bool ret = createHWNDRenderTarget(hdc);
        if (!ret) {
            ::ReleaseDC(w, hdc);
            hdc_ = nullptr;
        } else {
            hdc_ = hdc;
        }
        window_ = w;
        prepareResources();
    }

    GLCanvas::GLCanvas(HWND w, int width, int height) {
        bool ret = createOffscreenRenderTarget(w, width, height);
        window_ = w;
        rt_width_ = width;
        rt_height_ = height;
        prepareResources();
    }

    GLCanvas::~GLCanvas() {
        if (g_hPBuffer)
        {
            wglDeleteContext(g_hPBufferRC);
            wglReleasePbufferDCARB(g_hPBuffer, g_hPBufferDC);
            wglDestroyPbufferARB(g_hPBuffer);
            g_hPBufferRC = 0;
            g_hPBufferDC = 0;
            g_hPBuffer = 0;
        }

        if (gl_rc_) {
            wglDeleteContext(gl_rc_);
        }
        if (hdc_) {
            ::ReleaseDC(window_, hdc_);
        }
    }

    bool GLCanvas::createHWNDRenderTarget(HDC hdc) {
        PIXELFORMATDESCRIPTOR pfd = { 0 };
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

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

        return true;
    }

    bool GLCanvas::createOffscreenRenderTarget(HWND w, int width, int height) {
        // Even though we aren't going to be rendering the scene to the window
    // we still need to create a dummy rendering context in order to load the
    // pbuffer extensions and to create our pbuffer.

        PIXELFORMATDESCRIPTOR pfd = { 0 };

        // Don't bother with anything fancy here. This is just a dummy rendering
        // context so just ask for the bare minimum.
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cDepthBits = 16;
        pfd.iLayerType = PFD_MAIN_PLANE;

        HDC g_hDC;
        if (!(g_hDC = GetDC(w)))
            return false;

        int pf = ChoosePixelFormat(g_hDC, &pfd);

        if (!SetPixelFormat(g_hDC, pf, &pfd))
            return false;

        HGLRC g_hRC;
        if (!(g_hRC = wglCreateContext(g_hDC)))
            return false;

        if (!wglMakeCurrent(g_hDC, g_hRC))
            return false;

        if (!InitGLExtensions())
            return false;

        if (!InitPBuffer(g_hDC, width, height))
            return false;

        // Deactivate the dummy rendering context now that the pbuffer is created.
        wglMakeCurrent(g_hDC, 0);
        ReleaseDC(w, g_hDC);
        g_hDC = 0;

        // We are only doing off-screen rendering. So activate our pbuffer once.
        wglMakeCurrent(g_hPBufferDC, g_hPBufferRC);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(1, 1, 1, 0.5);
        glDisable(GL_DEPTH_TEST);

        glDisable(GL_CULL_FACE);

        return true;
    }

    bool GLCanvas::prepareResources() {
        auto image = Application::getImageLocFactory()->decodeFile(u"E:\\Test\\test.png", ImageOptions());
        if (image.isValid()) {
            auto frame = image.getFrames()[0];
            img_.width = frame->getPixelSize().width();
            img_.height = frame->getPixelSize().height();

            size_t stride;
            auto data = (const char*)frame->lockPixels(IAF_READ, &stride);
            img_.data.assign(data, data + stride * img_.height);
            frame->unlockPixels();
        }
        return true;
    }

    void GLCanvas::resize(int width, int height) {
        /*if (window_) {
            RECT rect;
            ::GetWindowRect(window_, &rect);

            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }*/

        if (width < 1) {
            width = 1;
        }
        if (height < 1) {
            height = 1;
        }

        rt_width_ = width;
        rt_height_ = height;

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
        glColor4f(1, 0, 0, 1);

        //drawRect({ 100, 100, 4, 4 });
        //drawLine({ 100.5, 100 }, { 100.5, 1000 });
        //drawCircle(200, 200, 100);

        drawBezier2({ 100, 100 }, { 300, 300 }, { 500, 100 });

        drawBitmap(img_.data.data(), img_.width, img_.height);

        //glFlush();

        if (hdc_) {
            ::SwapBuffers(hdc_);
        }
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

    bool GLCanvas::copy(void* pixels) {
        CopyPBufferToImage((BYTE*)pixels, rt_width_ * 4, rt_width_, rt_height_);
        return true;
    }

}