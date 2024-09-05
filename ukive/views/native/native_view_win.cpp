// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "native_view_win.h"

#include <fstream>

#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"


void ImagePreMultAlpha(BYTE* p, int width, int height, int stride)
{
    // The per pixel alpha blending API for layered windows deals with
    // pre-multiplied alpha values in the RGB channels. For further details see
    // the MSDN documentation for the BLENDFUNCTION structure. It basically
    // means we have to multiply each red, green, and blue channel in our image
    // with the alpha value divided by 255.
    //
    // Notes:
    // 1. ImagePreMultAlpha() needs to be called before every call to
    //    UpdateLayeredWindow() (in the RedrawLayeredWindow() function).
    //
    // 2. Must divide by 255.0 instead of 255 to prevent alpha values in range
    //    [1, 254] from causing the pixel to become black. This will cause a
    //    conversion from 'float' to 'BYTE' possible loss of data warning which
    //    can be safely ignored.

    BYTE* pPixel = NULL;

    if (width * 4 == stride)
    {
        // This is a special case. When the image width is already a multiple
        // of 4 the image does not require any padding bytes at the end of each
        // scan line. Consequently we do not need to address each scan line
        // separately. This is much faster than the below case where the image
        // width is not a multiple of 4.

        int totalBytes = width * height * 4;

        for (int i = 0; i < totalBytes; i += 4)
        {
            pPixel = &p[i];
            pPixel[0] = (BYTE)(pPixel[0] * (float)pPixel[3] / 255.0f);
            pPixel[1] = (BYTE)(pPixel[1] * (float)pPixel[3] / 255.0f);
            pPixel[2] = (BYTE)(pPixel[2] * (float)pPixel[3] / 255.0f);
        }
    } else
    {
        // Width of the image is not a multiple of 4. So padding bytes have
        // been included in the DIB's pixel data. Need to address each scan
        // line separately. This is much slower than the above case where the
        // width of the image is already a multiple of 4.

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                pPixel = &p[(y * stride) + (x * 4)];
                pPixel[0] = (BYTE)(pPixel[0] * (float)pPixel[3] / 255.0f);
                pPixel[1] = (BYTE)(pPixel[1] * (float)pPixel[3] / 255.0f);
                pPixel[2] = (BYTE)(pPixel[2] * (float)pPixel[3] / 255.0f);
            }
        }
    }
}

void SaveDIBToFile(
    const wchar_t* path,
    void* pixels, const BITMAPINFO& Info)
{
    BITMAPFILEHEADER Header;
    memset(&Header, 0, sizeof(Header));
    Header.bfType = 0x4D42;
    Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::ofstream hFile(path, std::ios::binary);
    if (hFile.is_open())
    {
        hFile.write((char*)&Header, sizeof(Header));
        hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
        hFile.write((const char*)pixels, 4 * Info.bmiHeader.biWidth * -Info.bmiHeader.biHeight);
        hFile.close();
    }
}

namespace ukive {
namespace win {

    NativeViewWin::NativeViewWin(Context c)
        : NativeViewWin(c, {}) {}

    NativeViewWin::NativeViewWin(Context c, AttrsRef attrs)
        : super(c, attrs)
    {
        memset(&mem_bmp_, 0, sizeof(mem_bmp_));
    }

    NativeViewWin::~NativeViewWin() {
    }

    void NativeViewWin::onAttachedToWindow(Window* w) {
        super::onAttachedToWindow(w);

        HWND parent = static_cast<WindowImplWin*>(w->getImpl())->getHandle();
        if (createChildLayeredWindow(parent)) {
            ::ShowWindow(hWnd_, SW_SHOW);
        }

        HDC parent_dc = GetDC(parent);
        mem_dc_ = CreateCompatibleDC(parent_dc);
        ReleaseDC(parent, parent_dc);

        gl_canvas_ = new GLCanvas(hWnd_, 16, 16);

        createDIBBitmap(16, 16, mem_dc_, &mem_bmp_);

        getWindow()->setAfterDrawCallback([this]() { onAfterDraw(); });
    }

    void NativeViewWin::onDetachFromWindow() {
        if (mem_dc_) {
            DeleteDC(mem_dc_);
            mem_dc_ = nullptr;
        }

        destroyDIBBitmap(&mem_bmp_);

        if (hWnd_) {
            ::UnregisterClassW(
                L"Ukive_WindowClass_Child_0",
                ::GetModuleHandleW(nullptr));
            ::DestroyWindow(hWnd_);
            hWnd_ = nullptr;
        }

        getWindow()->setAfterDrawCallback(nullptr);

        super::onDetachFromWindow();
    }

    void NativeViewWin::onBoundsChanged(
        const Rect& new_bounds,
        const Rect& old_bounds)
    {
        super::onBoundsChanged(new_bounds, old_bounds);

        if (hWnd_) {
            auto bounds = getBoundsInWindow();
            ::MoveWindow(
                hWnd_,
                bounds.x(),
                bounds.y(),
                bounds.width(),
                bounds.height(),
                FALSE);
            ShowWindow(hWnd_, SW_SHOW);

            delete gl_canvas_;
            gl_canvas_ = new GLCanvas(hWnd_, new_bounds.width(), new_bounds.height());
            gl_canvas_->resize(new_bounds.width(), new_bounds.height());

            destroyDIBBitmap(&mem_bmp_);
            createDIBBitmap(new_bounds.width(), new_bounds.height(), mem_dc_, &mem_bmp_);
        }
    }

    void NativeViewWin::onDraw(Canvas* canvas) {
        super::onDraw(canvas);
    }

    bool NativeViewWin::createChildLayeredWindow(HWND parent) {
        WNDCLASSEX wcex;
        wcex.style = 0;
        wcex.lpfnWndProc = NativeViewWin::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = ::GetModuleHandleW(nullptr);
        wcex.hIcon = nullptr;
        wcex.hIconSm = nullptr;
        wcex.hCursor = nullptr;
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"Ukive_WindowClass_Child_0";
        wcex.cbSize = sizeof(WNDCLASSEXW);

        auto atom = ::RegisterClassExW(&wcex);
        if (!atom) {
            return false;
        }

        HWND hWnd = ::CreateWindowExW(
            WS_EX_LAYERED,
            (LPCWSTR)atom,
            nullptr,
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            parent,
            nullptr,
            ::GetModuleHandleW(nullptr),
            nullptr);
        if (!hWnd) {
            ::UnregisterClassW(
                L"Ukive_WindowClass_Child_0",
                ::GetModuleHandleW(nullptr));
            return false;
        }

        hWnd_ = hWnd;
        return true;
    }

    // static
    bool NativeViewWin::createDIBBitmap(
        int width, int height, HDC mem_dc, DIBBitmap* out)
    {
        int stride = width * 4;

        void* pixels = nullptr;
        BITMAPINFO Info;
        memset(&Info, 0, sizeof(Info));
        Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        Info.bmiHeader.biWidth = width;
        Info.bmiHeader.biHeight = -height;
        Info.bmiHeader.biPlanes = 1;
        Info.bmiHeader.biBitCount = 32;
        Info.bmiHeader.biCompression = BI_RGB;
        Info.bmiHeader.biSizeImage = stride * height;
        HBITMAP section = CreateDIBSection(mem_dc, &Info, DIB_RGB_COLORS, (void**)&pixels, 0, 0);
        if (!section) {
            return false;
        }

        out->width = width;
        out->height = height;
        out->stride = stride;
        out->bmp = section;
        out->pixels = pixels;
        out->Info = Info;
        return true;
    }

    // static
    void NativeViewWin::destroyDIBBitmap(DIBBitmap* bmp) {
        if (bmp->bmp) {
            DeleteObject(bmp->bmp);
            memset(bmp, 0, sizeof(*bmp));
        }
    }

    void NativeViewWin::onAfterDraw() {
        gl_canvas_->render();

        if (hWnd_ && mem_dc_ && mem_bmp_.bmp) {
            RECT wr;
            ::GetWindowRect(hWnd_, &wr);

            gl_canvas_->copy(mem_bmp_.pixels);

            ImagePreMultAlpha(
                (BYTE*)mem_bmp_.pixels,
                mem_bmp_.width,
                mem_bmp_.height,
                mem_bmp_.stride);

            /*SaveDIBToFile(
                L"E:\\123.bmp",
                mem_bmp_.pixels,
                mem_bmp_.Info);*/

            HGDIOBJ prev_obj = SelectObject(mem_dc_, mem_bmp_.bmp);

            POINT zero = { 0, 0 };
            SIZE size = { mem_bmp_.width, mem_bmp_.height };
            POINT position = { wr.left, wr.top };
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            BOOL ret = ::UpdateLayeredWindow(
                hWnd_, nullptr, &position, &size, mem_dc_, &zero,
                RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
            if (ret == 0) {
                LOG(Log::ERR) << "Failed to update layered window: " << ::GetLastError();
            }

            SelectObject(mem_dc_, prev_obj);
        }
    }

    LRESULT CALLBACK NativeViewWin::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg) {
        case WM_ERASEBKGND:
            return 1;
        }

        return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

}
}