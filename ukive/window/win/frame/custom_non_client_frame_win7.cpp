// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/win/frame/custom_non_client_frame_win7.h"

#include <dwmapi.h>
#include <Windowsx.h>
#include <VersionHelpers.h>

#include "utils/log.h"

#include "ukive/window/win/window_impl_win.h"
#include "ukive/system/win/ui_utils_win.h"


namespace ukive {
namespace win {

    CustomNonClientFrameWin7::CustomNonClientFrameWin7()
        : window_(nullptr) {}

    int CustomNonClientFrameWin7::onNCCreate(WindowImplWin* w, bool* handled) {
        *handled = false;
        window_ = w;

        return TRUE;
    }

    int CustomNonClientFrameWin7::onNCDestroy(bool* handled) {
        *handled = false;

        window_ = nullptr;
        return FALSE;
    }

    void CustomNonClientFrameWin7::onTranslucentChanged(bool translucent) {
        /*if (translucent) {
            window_->setWindowStyle(WS_CAPTION, false, true);
        } else {
            if (!Application::isAeroEnabled()) {
                window_->setWindowStyle(WS_CAPTION, false, false);
            }
        }*/
    }

    void CustomNonClientFrameWin7::setExtraSpacingWhenMaximized(const Padding& spacing) {
        if (ext_sp_when_max_ != spacing) {
            ext_sp_when_max_ = spacing;
            window_->sendFrameChanged();
        }
    }

    void CustomNonClientFrameWin7::getClientInsets(Padding* insets, int* bottom_beyond) {
        assert(insets);
        // 从 Windows 7 到 Windows 10 1703，窗口渲染缓冲的大小需要
        // 与窗口客户区的大小完全一致，否则会出现模糊（可能是因为窗口交换缓冲设置的缩放模式为拉伸），
        // 重要的是底边非客户区设置的 -1 需要考虑在内。
        // Windows 10 1703 以上系统则不需要特别处理。
        int beyond = 1;

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
                insets->set(0, 0, 0, 0);
            }
            *bottom_beyond = 0;
        } else if (window_->isMaximized()) {
            *insets = getExtraSpacingWhenMaximized();
            *bottom_beyond = 0;
        } else {
            // 消除底边非客户区设置为 -1 的影响
            insets->set(0, 0, 0, 1);
            *bottom_beyond = (win::isAeroEnabled() ? beyond : 0);
        }
    }

    void CustomNonClientFrameWin7::getClientOffset(POINT* offset) {
        assert(offset);
        if (window_->isLayered()) {
            int border_thickness = getBorderThickness();
            if (window_->isMaximized()) {
                auto ext = getExtraSpacingWhenMaximized();
                offset->x = border_thickness + ext.start();
                offset->y = border_thickness + ext.top();
            } else {
                offset->x = 0;
                offset->y = 0;
            }
        } else {
            if (window_->isMaximized()) {
                auto ext = getExtraSpacingWhenMaximized();
                offset->x = ext.start();
                offset->y = ext.top();
            } else {
                offset->x = 0;
                offset->y = 0;
            }
        }
    }

    LRESULT CustomNonClientFrameWin7::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT CustomNonClientFrameWin7::onNCPaint(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_client)
    {
        *handled = !win::isAeroEnabled();
        *pass_to_client = false;
        return 0;
    }

    LRESULT CustomNonClientFrameWin7::onNCActivate(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_client)
    {
        *pass_to_client = false;
        if (window_->isMinimized()) {
            *handled = false;
            return 0;
        }

        // 当 Aero 被禁用，并且窗口处于非激活状态时，原生边框会露出来，
        // 因此需要在此时拦截掉该消息。
        *handled = !win::isAeroEnabled();
        return TRUE;
    }

    LRESULT CustomNonClientFrameWin7::onNCHitTest(
        WPARAM wParam, LPARAM lParam,
        bool* handled, bool* pass_to_client, POINT* p)
    {
        *handled = true;
        *pass_to_client = true;

        Point cp;
        cp.x(GET_X_LPARAM(lParam));
        cp.y(GET_Y_LPARAM(lParam));

        window_->convScreenToClient(&cp);

        p->x = cp.x();
        p->y = cp.y();

        return 0;
    }

    LRESULT CustomNonClientFrameWin7::onNCCalSize(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_client)
    {
        *handled = true;
        *pass_to_client = false;

        RECT* rect;
        if (wParam == TRUE) {
            rect = &reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0];
        } else {
            rect = reinterpret_cast<RECT*>(lParam);
        }

        if (window_->isLayered()) {
            auto ext = getExtraSpacingWhenMaximized();
            // 半透明窗口，一切都需要自绘，因此直接移除整个非客户区。
            if (window_->isMaximized()) {
                rect->left += ext.start();
                rect->top += ext.top();
                rect->right -= ext.end();
                rect->bottom -= ext.bottom();
            } else {
                rect->left += 0;
                rect->top += 0;
                rect->right -= 0;
                rect->bottom -= 0;
            }
        } else {
            if (window_->isMaximized()) {
                /*
                 * 最大化时，窗口四周似乎会增加额外的 SM_CXSIZEFRAME + SM_CXPADDEDBORDER 大小
                 * 的非客户区域，即使这里传 0 也一样，除非全传 0。但全传 0 的话，就不会有模糊效果。
                 * 如果用下面 else 里的方法传个 -1，虽然保留了模糊效果，但超出的部分在其他屏幕（多显示器）
                 * 上可见。于是乎全传 0 或用 -1 不可行。
                 * 那么这里就收缩客户区，把额外的空间减掉，相当于有了一点非客户区，但这里非客户区并不可见。
                 */
                auto ext = getExtraSpacingWhenMaximized();
                int border_thickness = getBorderThickness();
                rect->left += border_thickness + ext.start();
                rect->top += border_thickness + ext.top();
                rect->right -= border_thickness + ext.end();
                rect->bottom -= border_thickness + ext.bottom();
            } else {
                rect->left += 0;
                rect->top += 0;
                rect->right -= 0;
                /*
                 * 如果完全移除原生边框，则在调整窗口大小时，自绘边界会跳动，
                 * 而且在 Windows 7 上将看不到模糊效果。
                 * 因此让客户区在底部向外扩展 1 个像素，这样似乎可以保留边框，而且该扩展不可见。
                 *
                 * 但是这样也带来了一个问题，多屏的情况下，当窗口在某个屏幕上最大化时，相邻窗口的边缘
                 * 会出现窗口的边框。试图使用 WM_GETMINMAXINFO 解决问题无效。该问题只能在 WM_NCCALSIZE
                 * 中设置合适的非客户区大小来解决，或者创建窗口时不指定 WS_MAXIMIZEBOX
                 */
                rect->bottom -= -1;
            }
        }

        return wParam == TRUE ? WVR_REDRAW : 0;
    }

    LRESULT CustomNonClientFrameWin7::onNCMouseRange(
        UINT uMsg, WPARAM wParam, LPARAM lParam,
        bool* handled, bool* pass_to_client)
    {
        *handled = false;
        *pass_to_client = true;

        switch (uMsg) {
        case WM_NCRBUTTONUP:
            window_->showTitlebarMenu();
            break;

        case WM_NCMOUSEMOVE:
            *handled = true;
            return FALSE;

        default:
            break;
        }
        return FALSE;
    }

    LRESULT CustomNonClientFrameWin7::onNCMouseHover(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_client)
    {
        *handled = false;
        *pass_to_client = true;
        return 0;
    }

    LRESULT CustomNonClientFrameWin7::onNCMouseLeave(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_client)
    {
        *handled = false;
        *pass_to_client = true;
        return 0;
    }

    LRESULT CustomNonClientFrameWin7::onDwmCompositionChanged(bool* handled) {
        *handled = false;

        if (!window_->isTransparent() &&
            !window_->isLayered() &&
            win::isAeroEnabled())
        {
            // 切回 Aero 时，窗口阴影不会立刻显示，这里让它显示出来。
            MARGINS margins = { 0, 0, 0, 1 };
            HRESULT hr = ::DwmExtendFrameIntoClientArea(window_->getHandle(), &margins);
            ubassert(SUCCEEDED(hr));
            window_->sendFrameChanged();
        }

        return FALSE;
    }

    LRESULT CustomNonClientFrameWin7::onActivateAfterDwm() {
        if (!window_->isTransparent() &&
            !window_->isLayered())
        {
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

    LRESULT CustomNonClientFrameWin7::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return 0;
    }

    LRESULT CustomNonClientFrameWin7::onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) {
        //auto info = reinterpret_cast<MINMAXINFO*>(lParam);
        //auto win_impl = static_cast<WindowImplWin*>(window_->getImpl());
        //HMONITOR monitor = ::MonitorFromWindow(win_impl->getHandle(), MONITOR_DEFAULTTONEAREST);

        //MONITORINFO m_info;
        //m_info.cbSize = sizeof(MONITORINFO);
        //::GetMonitorInfoW(monitor, &m_info);
        //int width = m_info.rcWork.right - m_info.rcWork.left;
        //int height = m_info.rcWork.bottom - m_info.rcWork.top;

        //info->ptMaxPosition.x = 0;
        //info->ptMaxPosition.y = 0;
        //info->ptMaxSize.x = width;
        //info->ptMaxSize.y = height;
        ////info->ptMaxTrackSize.x = width;
        ////info->ptMaxTrackSize.y = height;

        //*handled = true;
        return 0;
    }

    int CustomNonClientFrameWin7::getBorderThickness() const {
        if (!window_->hasCaptain()) {
            return 0;
        }
        return ::GetSystemMetrics(SM_CXSIZEFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER);
    }

    Padding CustomNonClientFrameWin7::getExtraSpacingWhenMaximized() const {
        if (window_->isFullscreen()) {
            return {};
        }
        return ext_sp_when_max_;
    }

}
}