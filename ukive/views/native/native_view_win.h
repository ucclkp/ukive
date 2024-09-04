// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_NATIVE_NATIVE_VIEW_WIN_H_
#define UKIVE_VIEWS_NATIVE_NATIVE_VIEW_WIN_H_

#include "ukive/graphics/gl_canvas.h"
#include "ukive/views/view.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>


namespace ukive {
namespace win {

    class NativeViewWin : public View {
    public:
        explicit NativeViewWin(Context c);
        NativeViewWin(Context c, AttrsRef attrs);
        ~NativeViewWin();

    protected:
        void onAttachedToWindow(Window* w) override;
        void onDetachFromWindow() override;

        void onBoundsChanged(
            const Rect& new_bounds,
            const Rect& old_bounds) override;
        void onDraw(Canvas* canvas) override;

    private:
        using super = View;

        static LRESULT CALLBACK WndProc(
            HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        HWND hWnd_ = nullptr;
        GLCanvas* gl_canvas_ = nullptr;
    };

}
}

#endif  // UKIVE_VIEWS_NATIVE_NATIVE_VIEW_WIN_H_