// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "native_view_win.h"

#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"


namespace ukive {
namespace win {

    NativeViewWin::NativeViewWin(Context c)
        : NativeViewWin(c, {}) {}

    NativeViewWin::NativeViewWin(Context c, AttrsRef attrs)
        : super(c, attrs) {
    }

    NativeViewWin::~NativeViewWin() {
    }

    void NativeViewWin::onAttachedToWindow(Window* w) {
        super::onAttachedToWindow(w);

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

        auto parent_hwnd = static_cast<WindowImplWin*>(w->getImpl())->getHandle();

        auto atom = ::RegisterClassExW(&wcex);
        HWND hWnd = ::CreateWindowExW(
            WS_EX_LAYERED,
            (LPCWSTR)atom,
            nullptr,
            WS_CHILD,
            0, 0, 0, 0,
            parent_hwnd,
            nullptr,
            ::GetModuleHandleW(nullptr),
            nullptr);
        hWnd_ = hWnd;

        ::ShowWindow(hWnd_, SW_SHOW);
        BOOL ret = SetLayeredWindowAttributes(hWnd_, RGB(0xFF, 0xFF, 0xFF), 128, LWA_ALPHA);

        gl_canvas_ = new GLCanvas(hWnd_, 1, 1);
    }

    void NativeViewWin::onDetachFromWindow() {
        ::UnregisterClassW(
            L"Ukive_WindowClass_Child_0",
            ::GetModuleHandleW(nullptr));

        super::onDetachFromWindow();
    }

    void NativeViewWin::onBoundsChanged(
        const Rect& new_bounds,
        const Rect& old_bounds)
    {
        super::onBoundsChanged(new_bounds, old_bounds);

        if (hWnd_) {
            ::MoveWindow(
                hWnd_,
                new_bounds.x(),
                new_bounds.y(),
                new_bounds.width(),
                new_bounds.height(),
                TRUE);
            gl_canvas_->resize(new_bounds.width(), new_bounds.height());
        }
    }

    void NativeViewWin::onDraw(Canvas* canvas) {
        super::onDraw(canvas);

        gl_canvas_->render();
    }

    LRESULT CALLBACK NativeViewWin::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

}
}