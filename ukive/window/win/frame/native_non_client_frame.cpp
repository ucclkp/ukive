// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "native_non_client_frame.h"

#include "utils/log.h"

#include "ukive/window/win/window_impl_win.h"

#include <windowsx.h>


namespace ukive {
namespace win {

    int NativeNonClientFrame::onNcCreate(WindowImplWin* w, bool* handled) {
        *handled = false;
        window_ = w;
        return TRUE;
    }

    int NativeNonClientFrame::onNcDestroy(bool* handled) {
        *handled = false;
        window_ = nullptr;
        return FALSE;
    }

    void NativeNonClientFrame::setExtraSpacingWhenMaximized(const Padding& spacing) {
        if (spacing != ext_sp_when_max_) {
            ext_sp_when_max_ = spacing;
            window_->sendFrameChanged();
        }
    }

    void NativeNonClientFrame::getClientInsets(Padding* insets, int* bottom_beyond) {
        ubassert(insets);
        if (window_->isLayered()) {
            int border_thickness = getBorderThickness();
            if (window_->isMaximized()) {
                auto ext = getExtraSpacingWhenMaximized();
                insets->set(
                    border_thickness + ext.start(),
                    border_thickness + ext.top(),
                    border_thickness + ext.end(),
                    border_thickness + ext.bottom());
            } else {
                insets->set(
                    border_thickness,
                    border_thickness,
                    border_thickness,
                    border_thickness);
            }
        } else {
            insets->set(0, 0, 0, 0);
        }

        *bottom_beyond = 0;
    }

    void NativeNonClientFrame::getClientOffset(POINT* offset) {
        ubassert(offset);
        if (window_->isLayered() &&
            window_->isMaximized())
        {
            auto ext = getExtraSpacingWhenMaximized();
            int border_thickness = getBorderThickness();
            offset->x = border_thickness + ext.start();
            offset->y = border_thickness + ext.top();
        } else {
            offset->x = 0;
            offset->y = 0;
        }
    }

    LRESULT NativeNonClientFrame::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcHitTest(
        WPARAM wParam, LPARAM lParam, bool* handled,
        bool* pass_to_window, POINT* p)
    {
        if (window_->isLayered()) {
            *handled = true;
            *pass_to_window = true;

            Point cp;
            cp.x(GET_X_LPARAM(lParam));
            cp.y(GET_Y_LPARAM(lParam));

            window_->convScreenToClient(&cp);

            p->x = cp.x();
            p->y = cp.y();
        } else {
            *handled = false;
            *pass_to_window = false;
        }

        return HTNOWHERE;
    }

    LRESULT NativeNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (window_->isLayered()) {
            *handled = true;
            if (wParam == TRUE) {
                // 直接移除整个非客户区。
                auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                if (window_->isMaximized()) {
                    auto ext = getExtraSpacingWhenMaximized();
                    ncp->rgrc[0].left += ext.start();
                    ncp->rgrc[0].top += ext.top();
                    ncp->rgrc[0].right -= ext.end();
                    ncp->rgrc[0].bottom -= ext.bottom();
                } else {
                    ncp->rgrc[0].left += 0;
                    ncp->rgrc[0].top += 0;
                    ncp->rgrc[0].right -= 0;
                    ncp->rgrc[0].bottom -= 0;
                }
            }
            return 0;
        }

        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcRButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onNcRButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onDwmCompositionChanged(bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT NativeNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    int NativeNonClientFrame::getBorderThickness() const {
        if (!window_->hasCaptain()) {
            return 0;
        }
        int border_thickness = ::GetSystemMetrics(SM_CXSIZEFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER);
        return border_thickness;
    }

    Padding NativeNonClientFrame::getExtraSpacingWhenMaximized() const {
        if (window_->isFullscreen()) {
            return {};
        }
        return ext_sp_when_max_;
    }

}
}