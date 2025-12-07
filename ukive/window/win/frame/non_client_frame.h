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

        virtual int onNCCreate(WindowImplWin* w, bool* handled) = 0;
        virtual int onNCDestroy(bool* handled) = 0;
        virtual void onTranslucentChanged(bool translucent) = 0;

        virtual void setExtraSpacingWhenMaximized(const Padding& spacing) = 0;

        virtual void getClientInsets(Padding* insets, int* bottom_beyond) = 0;
        virtual void getClientOffset(POINT* offset) = 0;

        virtual LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled) = 0;

        virtual LRESULT onNCPaint(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) = 0;

        virtual LRESULT onNCActivate(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) = 0;

        virtual LRESULT onNCHitTest(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client, POINT* p) = 0;

        virtual LRESULT onNCCalSize(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) = 0;

        virtual LRESULT onNCMouseRange(
            UINT uMsg, WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) = 0;
        virtual LRESULT onNCMouseHover(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) = 0;
        virtual LRESULT onNCMouseLeave(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) = 0;

        virtual LRESULT onDwmCompositionChanged(bool* handled) = 0;
        virtual LRESULT onActivateAfterDwm() = 0;
        virtual LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
        virtual LRESULT onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) = 0;
    };

}
}

#endif  // UKIVE_WINDOW_WIN_FRAME_NON_CLIENT_FRAME_H_
