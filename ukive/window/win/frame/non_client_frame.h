// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WIN_FRAME_NON_CLIENT_FRAME_H_
#define UKIVE_WINDOW_WIN_FRAME_NON_CLIENT_FRAME_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ukive/graphics/padding.hpp"
#include "ukive/graphics/rect.hpp"


namespace ukive {
namespace win {

    class WindowImplWin;

    class NonClientFrame {
    public:
        virtual ~NonClientFrame() = default;

        virtual int onNcCreate(WindowImplWin* w, bool* handled) = 0;
        virtual int onNcDestroy(bool* handled) = 0;
        virtual void onTranslucentChanged(bool translucent) = 0;

        virtual void setExtraSpacingWhenMaximized(const Padding& spacing) = 0;

        virtual void getClientInsets(Padding* insets, int* bottom_beyond) = 0;
        virtual void getClientOffset(POINT* offset) = 0;

        virtual LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) = 0;

        virtual LRESULT onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onNcHitTest(
            WPARAM wParam, LPARAM lParam, bool* handled,
            bool* pass_to_window, POINT* p) = 0;
        virtual LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onNcRButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onNcRButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onDwmCompositionChanged(bool* handled) = 0;
        virtual LRESULT onActivateAfterDwm() = 0;
        virtual LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
    };

}
}

#endif  // UKIVE_WINDOW_WIN_FRAME_NON_CLIENT_FRAME_H_
