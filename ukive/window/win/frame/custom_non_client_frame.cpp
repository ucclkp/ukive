// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/win/frame/custom_non_client_frame.h"

#include <dwmapi.h>
#include <Windowsx.h>
#include <VersionHelpers.h>

#include "utils/log.h"

#include "ukive/system/win/dynamic_windows_api.h"
#include "ukive/window/win/window_impl_win.h"
#include "ukive/system/win/win10_version.h"


namespace ukive {
namespace win {

    CustomNonClientFrame::CustomNonClientFrame()
        : window_(nullptr) {}

    int CustomNonClientFrame::onNcCreate(WindowImplWin* w, bool* handled) {
        *handled = false;
        window_ = w;

        return TRUE;
    }

    int CustomNonClientFrame::onNcDestroy(bool* handled) {
        *handled = false;

        window_ = nullptr;
        return FALSE;
    }

    void CustomNonClientFrame::onTranslucentChanged(bool translucent) {
    }

    void CustomNonClientFrame::setExtraSpacingWhenMaximized(const Rect& rect) {
        if (ext_sp_when_max_ != rect) {
            ext_sp_when_max_ = rect;
            window_->sendFrameChanged();
        }
    }

    void CustomNonClientFrame::getClientInsets(RECT* rect, int* bottom_beyond) {
        ubassert(rect);
        // 从 Windows 7 到 Windows 10 1703，窗口渲染缓冲的大小需要
        // 与窗口客户区的大小完全一致，否则会出现模糊（可能是因为窗口交换缓冲设置的缩放模式为拉伸），
        // 重要的是底边非客户区设置的 -1 需要考虑在内。
        // Windows 10 1703 以上系统则不需要特别处理。
        static bool need_total = win::isWin10Ver1703OrGreater();
        int beyond = (need_total ? 0 : 1);

        if (window_->isMaximized()) {
            auto ext = getExtraSpacingWhenMaximized();
            rect->left = ext.left;
            rect->right = ext.right;
            rect->top = ext.top;
            rect->bottom = ext.bottom;
            *bottom_beyond = 0;
        } else {
            rect->left = 0;
            rect->right = 0;
            rect->top = 0;
            // 消除底边非客户区设置为 -1 的影响
            rect->bottom = 1;
            *bottom_beyond = beyond;
        }
    }

    void CustomNonClientFrame::getClientOffset(POINT* offset) {
        ubassert(offset);
        if (window_->isMaximized()) {
            auto ext = getExtraSpacingWhenMaximized();
            offset->x = ext.left;
            offset->y = ext.top;
        } else {
            offset->x = 0;
            offset->y = 0;
        }
    }

    LRESULT CustomNonClientFrame::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return 0;
    }

    LRESULT CustomNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (window_->isMinimized()) {
            *handled = false;
            return 0;
        }
        *handled = false;
        return TRUE;
    }

    LRESULT CustomNonClientFrame::onNcHitTest(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_window, POINT* p)
    {
        *handled = true;
        *pass_to_window = true;

        Point cp;
        cp.x() = GET_X_LPARAM(lParam);
        cp.y() = GET_Y_LPARAM(lParam);

        window_->convScreenToClient(&cp);

        p->x = cp.x();
        p->y = cp.y();

        return 0;
    }

    LRESULT CustomNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;

        RECT* rect;
        if (wParam == TRUE) {
            rect = &reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0];
        } else {
            rect = reinterpret_cast<RECT*>(lParam);
        }

        if (window_->isMaximized()) {
            /*
             * 这里采用了与 Windows 7 不同的方案。
             * Windows 7 的方案几近完美，除了一点：如果使用最大化方式打开窗口，
             * 则窗口会跳变，打开之后的最大化则不会出现该现象。
             * 这里如果直接全传 0，与 Windows 7 不同，模糊效果仍然存在，使用
             * 最大化方式打开窗口不会跳变，但关掉模糊效果之后会发现相邻屏幕仍有一部分窗口露出。
             * 因此这里使用 Windows 7 的方案，但 top 传 0 来解决跳变。
             *
             * *** 更新：没办法用传 0 来解决了。传 0 的话，窗口的那一侧会多出一点，没法让出
             * 自动隐藏应用栏的检测区了，于是还是用 Windows 7 的方案，最大化打开时跳过动画。
             *
             * *** 更新：无意间发现，使用 ShowWindow() 和 SetWindowPlacement() 的组合可以
             * 解决跳变的问题。
             */
            auto ext = getExtraSpacingWhenMaximized();
            int border_thickness = getBorderThickness();
            rect->left += border_thickness + ext.left;
            rect->top += border_thickness + ext.top;
            rect->right -= border_thickness + ext.right;
            rect->bottom -= border_thickness + ext.bottom;
        } else {
            rect->left += 0;
            rect->top += 0;
            rect->right -= 0;
            /*
             * 如果完全移除原生边框，则在调整窗口大小时，自绘边界会跳动，
             * 而且在 Windows 7 上将看不到模糊效果。
             * 因此让客户区在底部向外扩展 1 个像素，这样似乎可以保留边框，而且该扩展不可见。
             *
             * 从 Windows 10 2004 开始，即使使用该方法，窗口在调整大小时仍会出现轻微跳动。
             *
             * 但是这样也带来了一个问题，多屏的情况下，当窗口在某个屏幕上最大化时，相邻窗口的边缘
             * 会出现窗口的边框。试图使用 WM_GETMINMAXINFO 解决问题无效。该问题只能在 WM_NCCALSIZE
             * 中设置合适的非客户区大小来解决，或者创建窗口时不指定 WS_MAXIMIZEBOX.
             */
            rect->bottom -= -1;
        }

        return wParam == TRUE ? WVR_REDRAW : 0;
    }

    LRESULT CustomNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onNcRButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onNcRButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        window_->showTitlebarMenu();
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onDwmCompositionChanged(bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onActivateAfterDwm() {
        if (!window_->isTransparent() && !window_->isFullscreen()) {
            // 让移除了标准边框的窗口能够显示原生阴影。
            // 在 Windows 7 上，该方法将会在窗口底部缩进一个像素，这一像素的横线具有毛玻璃效果；
            // 在 Windows 8 及以上系统，该方法将会在窗口底部缩进一个像素，这一像素是灰色的。
            // 这一像素是可以被覆盖的，如果覆盖的颜色完全不透明的话。
            MARGINS margins = { 0, 0, 0, 1 };
            HRESULT hr = ::DwmExtendFrameIntoClientArea(window_->getHandle(), &margins);
            ubassert(SUCCEEDED(hr));
            window_->sendFrameChanged();
        }

        return 0;
    }

    LRESULT CustomNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return 0;
    }

    LRESULT CustomNonClientFrame::onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) {
        /*auto info = reinterpret_cast<MINMAXINFO*>(lParam);

        HMONITOR monitor = ::MonitorFromWindow(window_->getHandle(), MONITOR_DEFAULTTONEAREST);

        MONITORINFO m_info;
        m_info.cbSize = sizeof(MONITORINFO);
        ::GetMonitorInfoW(monitor, &m_info);
        int width = m_info.rcWork.right - m_info.rcWork.left;
        int height = m_info.rcWork.bottom - m_info.rcWork.top;

        info->ptMaxPosition.x = 0;
        info->ptMaxPosition.y = 0;
        info->ptMaxSize.x = width;
        info->ptMaxSize.y = height;
        info->ptMaxTrackSize.x = width;
        info->ptMaxTrackSize.y = height;

        *handled = true;*/
        return 0;
    }

    int CustomNonClientFrame::getBorderThickness() const {
        if (!window_->hasCaptain()) {
            return 0;
        }

        static bool per_monitor = win::isWin10Ver1607OrGreater();
        if (per_monitor) {
            auto dpi = win::UDGetDpiForWindow(window_->getHandle());
            if (dpi != 0) {
                return win::UDGetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi)
                    + win::UDGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
            }
        }

        return ::GetSystemMetrics(SM_CXSIZEFRAME)
            + ::GetSystemMetrics(SM_CXPADDEDBORDER);
    }

    Rect CustomNonClientFrame::getExtraSpacingWhenMaximized() const {
        if (window_->isFullscreen()) {
            return {};
        }
        return ext_sp_when_max_;
    }

}
}