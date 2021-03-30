// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WIN_FRAME_CUSTOM_NON_CLIENT_FRAME_WIN7_H_
#define UKIVE_WINDOW_WIN_FRAME_CUSTOM_NON_CLIENT_FRAME_WIN7_H_

#include "ukive/window/win/frame/non_client_frame.h"


namespace ukive {

    /**
     * Windows 7 专用的去除非客户区的方案。
     */
    class CustomNonClientFrameWin7 : public NonClientFrame {
    public:
        CustomNonClientFrameWin7();

        int onNcCreate(WindowImplWin* w, bool* handled) override;
        int onNcDestroy(bool* handled) override;
        void onTranslucentChanged(bool translucent) override;

        void setExtraSpacingWhenMaximized(const Rect& rect) override;

        void getClientInsets(RECT* rect, int* bottom_beyond) override;
        void getClientOffset(POINT* offset) override;

        LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;

        LRESULT onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcHitTest(
            WPARAM wParam, LPARAM lParam, bool* handled,
            bool* pass_to_window, POINT* p) override;
        LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcRButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcRButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onDwmCompositionChanged(bool* handled) override;
        LRESULT onActivateAfterDwm() override;
        LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) override;

    private:
        int getBorderThickness() const;
        Rect getExtraSpacingWhenMaximized() const;

        Rect ext_sp_when_max_;
        WindowImplWin* window_;
    };

}

#endif  // UKIVE_WINDOW_WIN_FRAME_CUSTOM_NON_CLIENT_FRAME_WIN7_H_