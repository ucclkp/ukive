// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WIN_FRAME_CUSTOM_NON_CLIENT_FRAME_WIN7_H_
#define UKIVE_WINDOW_WIN_FRAME_CUSTOM_NON_CLIENT_FRAME_WIN7_H_

#include "ukive/window/win/frame/non_client_frame.h"


namespace ukive {
namespace win {

    /**
     * Windows 7 专用的去除非客户区的方案。
     */
    class CustomNonClientFrameWin7 : public NonClientFrame {
    public:
        CustomNonClientFrameWin7();

        int onNCCreate(WindowImplWin* w, bool* handled) override;
        int onNCDestroy(bool* handled) override;
        void onTranslucentChanged(bool translucent) override;

        void setExtraSpacingWhenMaximized(const Padding& spacing) override;

        void getClientInsets(Padding* insets, int* bottom_beyond) override;
        void getClientOffset(POINT* offset) override;

        LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled) override;

        LRESULT onNCPaint(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) override;

        LRESULT onNCActivate(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) override;

        LRESULT onNCHitTest(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client, POINT* p) override;

        LRESULT onNCCalSize(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) override;

        LRESULT onNCMouseRange(
            UINT uMsg, WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) override;
        LRESULT onNCMouseHover(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) override;
        LRESULT onNCMouseLeave(
            WPARAM wParam, LPARAM lParam,
            bool* handled, bool* pass_to_client) override;

        LRESULT onDwmCompositionChanged(bool* handled) override;
        LRESULT onActivateAfterDwm() override;
        LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) override;

    private:
        int getBorderThickness() const;
        Padding getExtraSpacingWhenMaximized() const;

        Padding ext_sp_when_max_;
        WindowImplWin* window_;
    };

}
}

#endif  // UKIVE_WINDOW_WIN_FRAME_CUSTOM_NON_CLIENT_FRAME_WIN7_H_