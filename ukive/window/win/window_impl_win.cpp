// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "window_impl_win.h"

#include <algorithm>

#include <dwmapi.h>
#include <shellapi.h>
#include <tchar.h>
#include <tpcshrd.h>
#include <VersionHelpers.h>

#include "utils/log.h"
#include "utils/message/win/message_pump_ui_win.h"
#include "utils/numbers.hpp"
#include "utils/time_utils.h"

#include "ukive/app/application.h"
#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/dirty_region.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/win/colors/color_manager_win.h"
#include "ukive/graphics/win/display_manager_win.h"
#include "ukive/graphics/win/vsync_provider_win.h"
#include "ukive/system/win/dynamic_windows_api.h"
#include "ukive/system/win/reg_manager.h"
#include "ukive/system/win/ui_utils_win.h"
#include "ukive/system/win/win_app_bar.h"
#include "ukive/system/win/win10_version.h"
#include "ukive/window/context.h"
#include "ukive/window/window_dpi_utils.h"
#include "ukive/window/window_listener.h"
#include "ukive/window/window_native_delegate.h"
#include "ukive/window/window_types.h"
#include "ukive/window/win/frame/custom_non_client_frame.h"
#include "ukive/window/win/frame/custom_non_client_frame_win7.h"
#include "ukive/window/win/frame/non_client_frame.h"
#include "ukive/window/win/frame/native_non_client_frame.h"
#include "ukive/window/win/window_class_manager.h"

#define MI_WP_SIGNATURE  0xFF515700
#define SIGNATURE_MASK   0xFFFFFF00
#define TOUCH_PEN_MASK   0x80


namespace ukive {
namespace win {

    const int kDefaultX = 0;
    const int kDefaultY = 0;
    const int kDefaultWidth = 400;
    const int kDefaultHeight = 400;
    const wchar_t kDefaultTitle[] = L"Ukive Window";

    const int kDefaultClassStyle = CS_HREDRAW | CS_VREDRAW;
    // WS_OVERLAPPEDWINDOW 中包含 WS_MAXIMIZEBOX，该标志会使窗口在最大化时
    // 为其上下左右添加额外的边框。因此如果要自绘整个窗口的话，这一点需要进行处理。
    const int kDefaultWindowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    const int kDefaultWindowExStyle = WS_EX_APPWINDOW;


    WindowImplWin::WindowImplWin(WindowNativeDelegate* d)
        : delegate_(d),
          hWnd_(nullptr),
          blur_rgn_(nullptr),
          cursor_(Cursor::ARROW),
          x_(kDefaultX),
          y_(kDefaultY),
          prev_x_(kDefaultX),
          prev_y_(kDefaultY),
          width_(kDefaultWidth),
          height_(kDefaultHeight),
          prev_width_(kDefaultWidth),
          prev_height_(kDefaultHeight),
          title_(kDefaultTitle),
          is_created_(false),
          is_showing_(false),
          is_first_nccalc_(true),
          close_methods_(WINDOW_CLOSE_BY_BUTTON | WINDOW_CLOSE_BY_MENU)
    {
        ubassert(delegate_);

        saved_place_.length = sizeof(WINDOWPLACEMENT);
    }

    WindowImplWin::~WindowImplWin() {
        if (blur_rgn_) {
            ::DeleteObject(blur_rgn_);
        }
    }

    bool WindowImplWin::initialize(HWND parent) {
        auto context = delegate_->onGetContext();
        getThemeConfig(&context.getCurrentThemeConfig());

        ClassInfo info;
        info.style = kDefaultClassStyle;

        std::u16string icon_name, small_icon_name;
        if (delegate_->onGetWindowIconName(&icon_name, &small_icon_name)) {
            std::wstring w_in(icon_name.begin(), icon_name.end());
            std::wstring w_sin(small_icon_name.begin(), small_icon_name.end());

            info.icon = ::LoadIcon(nullptr, w_in.c_str());
            info.icon_small = ::LoadIcon(nullptr, w_sin.c_str());
        } else {
            info.icon = info.icon_small = ::LoadIcon(nullptr, IDI_WINLOGO);
        }

        info.cursor = ::LoadCursor(nullptr, IDC_ARROW);

        int win_style = kDefaultWindowStyle;
        int win_ex_style = kDefaultWindowExStyle;

        if (context.getTranslucentType() & TRANS_LAYERED) {
            win_ex_style |= WS_EX_LAYERED;
        }
        if (is_keep_on_top_) {
            win_ex_style |= WS_EX_TOPMOST;
        }
        if (!is_show_in_task_bar_) {
            win_ex_style &= ~WS_EX_APPWINDOW;
            win_ex_style |= WS_EX_TOOLWINDOW;
        }
        if (is_ignore_mouse_events_) {
            // https://devblogs.microsoft.com/oldnewthing/20121217-00/?p=5823
            win_ex_style |= WS_EX_TRANSPARENT;
        }

        ATOM atom = WindowClassManager::getInstance()->retrieveWindowClass(info);
        if (atom == 0) {
            return false;
        }

        if (!is_resizable_) {
            win_style &= ~WS_THICKFRAME;
        }
        if (!is_maximizable_) {
            win_style &= ~WS_MAXIMIZEBOX;
        }
        if (!is_minimizable_) {
            win_style &= ~WS_MINIMIZEBOX;
        }

        // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-features#owned-windows
        HWND hWnd = ::CreateWindowEx(
            win_ex_style,
            reinterpret_cast<wchar_t*>(atom),
            title_.c_str(), win_style,
            x_, y_, width_, height_,
            parent, nullptr, ::GetModuleHandle(nullptr), this);
        if (::IsWindow(hWnd) == FALSE) {
            LOG(Log::ERR) << "failed to create window.";
            return false;
        }

        if (is_fullscreen_) {
            DWORD style = DWORD(::GetWindowLongPtr(hWnd, GWL_STYLE));
            enableFullscreen(style);
        }

        updateAppBarAwareRect();

        if ((context.getTranslucentType() & TRANS_LAYERED) || !win::isAeroEnabled()) {
            setWindowRectShape(true);
        }

        if (Application::getOptions().is_auto_dpi_scale) {
            context.setScale(1);
            context.setAutoScale(getUserScale());
        } else {
            context.setScale(getUserScale());
            context.setAutoScale(1);
        }

        context.setDefaultDpi(kDefaultDpi);

        is_created_ = true;
        delegate_->onCreated();
        return true;
    }

    bool WindowImplWin::init(const InitParams& params) {
        if (is_created_) {
            return true;
        }

        HWND parent_hwnd;
        if (params.parent && params.parent->isCreated()) {
            parent_hwnd = static_cast<WindowImplWin*>(params.parent)->getHandle();
        } else {
            parent_hwnd = nullptr;
        }

        init_params_ = params;

        if (!initialize(parent_hwnd)) {
            init_params_ = InitParams();
            return false;
        }

        return true;
    }

    void WindowImplWin::show() {
        if (!is_created_) {
            return;
        }

        ::ShowWindow(hWnd_, SW_SHOWDEFAULT);
        is_showing_ = true;
        is_first_show_maximized_ = false;
    }

    void WindowImplWin::hide() {
        if (!is_created_) {
            return;
        }

        if (!is_showing_) {
            return;
        }

        ::ShowWindow(hWnd_, SW_HIDE);
        is_showing_ = false;
        is_first_show_maximized_ = false;
    }

    void WindowImplWin::minimize() {
        if (!is_created_) {
            return;
        }
        if (!isMinimizable()) {
            return;
        }

        ::ShowWindow(hWnd_, SW_MINIMIZE);
        is_showing_ = true;
        is_first_show_maximized_ = false;
    }

    void WindowImplWin::maximize() {
        if (!is_created_) {
            return;
        }

        if (!isMaximizable()) {
            return;
        }

        if (is_first_show_maximized_) {
            /**
             * 防止窗口最大化时顶部跳变。
             * 详见 CustomNonClientFrame 类的 onNcCalSize 方法。
             */
            ::ShowWindow(hWnd_, SW_SHOWDEFAULT);

            WINDOWPLACEMENT place;
            place.length = sizeof(WINDOWPLACEMENT);
            ::GetWindowPlacement(hWnd_, &place);

            place.flags = 0;
            place.showCmd = SW_MAXIMIZE;

            ::SetWindowPlacement(hWnd_, &place);
            is_first_show_maximized_ = false;
        }

        ::ShowWindow(hWnd_, SW_MAXIMIZE);
        is_showing_ = true;
    }

    void WindowImplWin::restore() {
        if (!is_created_) {
            return;
        }

        if (isFullscreen() || (!isMinimized() && !isMaximized())) {
            return;
        }

        ::ShowWindow(hWnd_, SW_RESTORE);
        is_showing_ = true;
        is_first_show_maximized_ = false;
    }

    void WindowImplWin::focus() {
        if (is_created_) {
            ::SetForegroundWindow(hWnd_);
        }
    }

    void WindowImplWin::close() {
        if (!is_created_) {
            return;
        }

        if (!isClosable()) {
            return;
        }

        if (delegate_->onClose()) {
            BOOL ret = ::DestroyWindow(hWnd_);
            if (ret == 0) {
                LOG(Log::ERR) << "Failed to destroy window.";
            }
        }
    }

    void WindowImplWin::center() {
        auto display_bounds = DisplayWin::fromWindowImpl(this)->getPixelWorkArea();
        x_ = int(std::round((display_bounds.width() - width_) / 2.f));
        y_ = int(std::round((display_bounds.height() - height_) / 2.f));

        if (::IsWindow(hWnd_)) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImplWin::invalidate(const DirtyRegion& region) {
        delegate_->onPostRender();
    }

    void WindowImplWin::requestLayout() {
        delegate_->onPostLayout();
    }

    void WindowImplWin::doLayout() {
        delegate_->onLayout();
    }

    void WindowImplWin::doDraw(const DirtyRegion& region) {
        DirtyRegion dirty_region(region);
        ukive::scaleFromNative(this, &dirty_region.rect0);
        ukive::scaleFromNative(this, &dirty_region.rect1);

        delegate_->onDraw(dirty_region);
    }

    void WindowImplWin::setTitle(const std::u16string_view& title) {
        title_ = std::wstring(title.begin(), title.end());
        if (::IsWindow(hWnd_)) {
            ::SetWindowText(hWnd_, title_.c_str());
        }
    }

    void WindowImplWin::setBounds(int x, int y, int width, int height) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        if (::IsWindow(hWnd_)) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImplWin::setCurrentCursor(Cursor cursor) {
        HCURSOR native_cursor = nullptr;
        switch (cursor) {
        case Cursor::ARROW:    native_cursor = ::LoadCursor(nullptr, IDC_ARROW);    break;
        case Cursor::IBEAM:    native_cursor = ::LoadCursor(nullptr, IDC_IBEAM);    break;
        case Cursor::WAIT:     native_cursor = ::LoadCursor(nullptr, IDC_WAIT);     break;
        case Cursor::CROSS:    native_cursor = ::LoadCursor(nullptr, IDC_CROSS);    break;
        case Cursor::UPARROW:  native_cursor = ::LoadCursor(nullptr, IDC_UPARROW);  break;
        case Cursor::SIZENWSE: native_cursor = ::LoadCursor(nullptr, IDC_SIZENWSE); break;
        case Cursor::SIZENESW: native_cursor = ::LoadCursor(nullptr, IDC_SIZENESW); break;
        case Cursor::SIZEWE:   native_cursor = ::LoadCursor(nullptr, IDC_SIZEWE);   break;
        case Cursor::SIZENS:   native_cursor = ::LoadCursor(nullptr, IDC_SIZENS);   break;
        case Cursor::SIZEALL:  native_cursor = ::LoadCursor(nullptr, IDC_SIZEALL);  break;
        case Cursor::_NO:      native_cursor = ::LoadCursor(nullptr, IDC_NO);       break;
        case Cursor::HAND:     native_cursor = ::LoadCursor(nullptr, IDC_HAND);     break;
        case Cursor::APPSTARTING: native_cursor = ::LoadCursor(nullptr, IDC_APPSTARTING); break;
        case Cursor::HELP:     native_cursor = ::LoadCursor(nullptr, IDC_HELP);     break;
        default:               native_cursor = ::LoadCursor(nullptr, IDC_ARROW);    break;
        }

        if (!native_cursor) {
            LOG(Log::ERR) << "Null native cursor.";
            return;
        }

        cursor_ = cursor;

        ::SetCursor(native_cursor);
    }

    void WindowImplWin::setTranslucentType(TranslucentType type) {
        // TRANS_LAYERED 和其他选项互斥
        if ((type & TRANS_LAYERED) && type != TRANS_LAYERED) {
            ubassert(false);
            return;
        }

        if (!win::isActivated()) {
            type = TRANS_OPAQUE;
        }

        if ((type & TRANS_LAYERED) && IsWindows8OrGreater()) {
            type = TRANS_TRANSPARENT;
        }

        auto context = delegate_->onGetContext();
        if (context.getTranslucentType() == type) {
            return;
        }

        if (::IsWindow(hWnd_)) {
            disablePrevTranslucent(type);
            enableCurTranslucent(type);

            context.setTranslucentType(type);
            delegate_->onUpdateContext(Context::TRANSLUCENT_CHANGED);
        } else {
            context.setTranslucentType(type);
        }
    }

    void WindowImplWin::setFullscreen(bool enabled) {
        if (is_fullscreen_ == enabled) {
            return;
        }
        is_fullscreen_ = enabled;

        if (!is_created_) {
            return;
        }

        DWORD style = DWORD(::GetWindowLongPtr(hWnd_, GWL_STYLE));
        if (enabled) {
            enableFullscreen(style);
        } else {
            style |= WS_OVERLAPPEDWINDOW;
            if (!is_resizable_) {
                style &= ~WS_THICKFRAME;
            }
            if (!is_maximizable_) {
                style &= ~WS_MAXIMIZEBOX;
            }
            if (!is_minimizable_) {
                style &= ~WS_MINIMIZEBOX;
            }

            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style);
            ::SetWindowPlacement(hWnd_, &saved_place_);
            sendFrameChanged();
        }
    }

    void WindowImplWin::setResizable(bool enabled) {
        if (is_resizable_ == enabled) {
            return;
        }
        is_resizable_ = enabled;

        if (!::IsWindow(hWnd_)) {
            return;
        }

        auto style = ::GetWindowLongPtr(hWnd_, GWL_STYLE);
        if (enabled && !isFullscreen()) {
            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style | WS_THICKFRAME);
        } else {
            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style & ~WS_THICKFRAME);
        }
    }

    void WindowImplWin::setMaximizable(bool enabled) {
        if (is_maximizable_ == enabled) {
            return;
        }
        is_maximizable_ = enabled;

        if (!is_created_) {
            return;
        }

        if (!enabled && isMaximized()) {
            restore();
        }

        delegate_->onWindowButtonChanged(WindowButton::Max);

        auto style = ::GetWindowLongPtr(hWnd_, GWL_STYLE);
        if (enabled && !isFullscreen()) {
            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style | WS_MAXIMIZEBOX);
        } else {
            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style & ~WS_MAXIMIZEBOX);
        }
    }

    void WindowImplWin::setMinimizable(bool enabled) {
        if (is_minimizable_ == enabled) {
            return;
        }
        is_minimizable_ = enabled;

        if (!is_created_) {
            return;
        }

        if (!enabled && isMinimized()) {
            restore();
        }
        delegate_->onWindowButtonChanged(WindowButton::Min);

        auto style = ::GetWindowLongPtr(hWnd_, GWL_STYLE);
        if (enabled && !isFullscreen()) {
            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style | WS_MINIMIZEBOX);
        } else {
            ::SetWindowLongPtr(hWnd_, GWL_STYLE, style & ~WS_MINIMIZEBOX);
        }
    }

    void WindowImplWin::setClosable(bool enabled) {
        if (is_closable_ == enabled) {
            return;
        }
        is_closable_ = enabled;

        if (!is_created_) {
            return;
        }

        delegate_->onWindowButtonChanged(WindowButton::Close);
    }

    void WindowImplWin::setCloseMethods(uint32_t methods) {
        if (methods == close_methods_) {
            return;
        }
        close_methods_ = methods;
    }

    void WindowImplWin::setKeepOnTop(bool enabled) {
        if (enabled == is_keep_on_top_) {
            return;
        }
        is_keep_on_top_ = enabled;

        if (!::IsWindow(hWnd_)) {
            return;
        }

        ::SetWindowPos(
            hWnd_,
            enabled ? HWND_TOPMOST : HWND_NOTOPMOST,
            0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    void WindowImplWin::setShowInTaskBar(bool enabled) {
        if (enabled == is_show_in_task_bar_) {
            return;
        }
        is_show_in_task_bar_ = enabled;

        if (!::IsWindow(hWnd_)) {
            return;
        }

        auto cur_ex_style = ::GetWindowLongPtr(hWnd_, GWL_EXSTYLE);
        if (enabled) {
            cur_ex_style |= WS_EX_APPWINDOW;
            cur_ex_style &= ~WS_EX_TOOLWINDOW;
        } else {
            cur_ex_style &= ~WS_EX_APPWINDOW;
            cur_ex_style |= WS_EX_TOOLWINDOW;
        }
        ::SetWindowLongPtr(hWnd_, GWL_EXSTYLE, cur_ex_style);
    }

    void WindowImplWin::setIgnoreMouseEvents(bool ignore) {
        if (ignore == is_ignore_mouse_events_) {
            return;
        }
        is_ignore_mouse_events_ = ignore;

        if (!::IsWindow(hWnd_)) {
            return;
        }

        auto cur_ex_style = ::GetWindowLongPtr(hWnd_, GWL_EXSTYLE);
        if (ignore) {
            cur_ex_style |= WS_EX_TRANSPARENT;
        } else {
            cur_ex_style &= ~WS_EX_TRANSPARENT;
        }
        ::SetWindowLongPtr(hWnd_, GWL_EXSTYLE, cur_ex_style);
    }

    std::u16string WindowImplWin::getTitle() const {
        return std::u16string(title_.begin(), title_.end());
    }

    Rect WindowImplWin::getBounds() const {
        return Rect(x_, y_, width_, height_);
    }

    Rect WindowImplWin::getContentBounds() const {
        return Rect(
            getClientOffsetX(), getClientOffsetY(),
            getContentWidth(), getContentHeight());
    }

    int WindowImplWin::getClientOffsetX() const {
        POINT p;
        non_client_frame_->getClientOffset(&p);
        return p.x;
    }

    int WindowImplWin::getClientOffsetY() const {
        POINT p;
        non_client_frame_->getClientOffset(&p);
        return p.y;
    }

    int WindowImplWin::getClientWidth() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        int width = rect.right - rect.left;
        if (width == 0) {
            return 0;
        }

        int bb;
        Padding insets;
        non_client_frame_->getClientInsets(&insets, &bb);
        width -= insets.hori();

        return (std::max)(width, 0);
    }

    int WindowImplWin::getClientHeight() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        int height = rect.bottom - rect.top;
        if (height == 0) {
            return 0;
        }

        int bb;
        Padding insets;
        non_client_frame_->getClientInsets(&insets, &bb);
        height -= insets.vert();

        return (std::max)(height, 0);
    }

    int WindowImplWin::getContentWidth() const {
        int width;
        if (delegate_->onGetContext().getTranslucentType() & TRANS_LAYERED) {
            if (isMaximized()) {
                width = getClientWidth();
            } else {
                width = width_;
            }
        } else {
            width = getClientWidth();
        }
        return width;
    }

    int WindowImplWin::getContentHeight() const {
        int height;
        if (delegate_->onGetContext().getTranslucentType() & TRANS_LAYERED) {
            if (isMaximized()) {
                height = getClientHeight();
            } else {
                height = height_;
            }
        } else {
            height = getClientHeight();
        }
        return height;
    }

    int WindowImplWin::getTextureWidth() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        int width = rect.right - rect.left;
        return (std::max)(width, 0);
    }

    int WindowImplWin::getTextureHeight() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        int height = rect.bottom - rect.top;

        int bb;
        Padding insets;
        non_client_frame_->getClientInsets(&insets, &bb);
        height -= bb;

        return (std::max)(height, 0);
    }

    float WindowImplWin::getUserScale() const {
        static bool is_win10_1607_or_above = win::isWin10Ver1607OrGreater();
        if (::IsWindow(hWnd_) && is_win10_1607_or_above) {
            UINT dpi = win::UDGetDpiForWindow(hWnd_);
            if (dpi > 0) {
                return utl::num_cast<float>(dpi) / kDefaultDpi;
            }
        }

        float sx, sy;
        DisplayWin::fromWindowImpl(this)->getUserScale(&sx, &sy);
        return sx;
    }

    void WindowImplWin::getThemeConfig(ThemeConfig* config) const {
        config->type = 0;
        config->has_color = win::isAeroEnabled();

        if (config->has_color) {
            DWORD _color;
            BOOL opaque_blend;
            HRESULT hr = ::DwmGetColorizationColor(&_color, &opaque_blend);
            if (SUCCEEDED(hr)) {
                config->primary_color = Color::ofARGB(_color);
            }
        }

        RegManager::SysThemeConfig info;
        if (RegManager::getSysThemeConfig(&info)) {
            config->light_theme = info.apps_use_light_theme;
            config->transparency_enabled = info.transparency_enabled;
        }
    }

    bool WindowImplWin::getICMProfilePath(std::wstring* path) const {
        HDC hdc = ::GetDC(hWnd_);
        if (!hdc) {
            return false;
        }

        bool ret = ColorManagerWin::getICMProfile(hdc, path);

        ::ReleaseDC(hWnd_, hdc);
        return ret;
    }

    HWND WindowImplWin::getHandle() const {
        return hWnd_;
    }

    Cursor WindowImplWin::getCurrentCursor() const {
        return cursor_;
    }

    WindowFrameType WindowImplWin::getFrameType() const {
        return init_params_.frame_type;
    }

    uint32_t WindowImplWin::getCloseMethods() const {
        return close_methods_;
    }

    bool WindowImplWin::isCreated() const {
        return is_created_;
    }

    bool WindowImplWin::isShowing() const {
        return is_showing_;
    }

    bool WindowImplWin::isMinimized() const {
        if (::IsWindow(hWnd_)) {
            return ::IsIconic(hWnd_) != 0;
        }
        return false;
    }

    bool WindowImplWin::isMaximized() const {
        if (::IsWindow(hWnd_)) {
            return ::IsZoomed(hWnd_) != 0;
        }
        return false;
    }

    bool WindowImplWin::isFullscreen() const {
        return is_fullscreen_;
    }

    bool WindowImplWin::isResizable() const {
        return is_resizable_ && !isMaximized() && !isMinimized() && !isFullscreen();
    }

    bool WindowImplWin::isMaximizable() const {
        return is_maximizable_ && !isMaximized() && !isFullscreen();
    }

    bool WindowImplWin::isMinimizable() const {
        return is_minimizable_ && !isMinimized() && !isFullscreen();
    }

    bool WindowImplWin::isClosable() const {
        return is_closable_;
    }

    bool WindowImplWin::isKeepOnTop() const {
        return is_keep_on_top_;
    }

    bool WindowImplWin::isShowInTaskBar() const {
        return is_show_in_task_bar_;
    }

    bool WindowImplWin::isIgnoreMouseEvents() const {
        return is_ignore_mouse_events_;
    }

    bool WindowImplWin::hasSizeBorder() const {
        bool no_size_border = (delegate_->onGetContext().getTranslucentType() & TRANS_LAYERED) ||
            init_params_.frame_type == WINDOW_FRAME_CUSTOM;
        return !no_size_border;
    }

    bool WindowImplWin::isPopup() const {
        if (::IsWindow(hWnd_)) {
            return ::GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_POPUP;
        }
        return false;
    }

    bool WindowImplWin::isLayered() const {
        return delegate_->onGetContext().getTranslucentType() & TRANS_LAYERED;
    }

    bool WindowImplWin::isTransparent() const {
        return delegate_->onGetContext().getTranslucentType() & TRANS_TRANSPARENT;
    }

    bool WindowImplWin::hasCaptain() const {
        if (::IsWindow(hWnd_)) {
            return ::GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_CAPTION;
        }
        return false;
    }

    bool WindowImplWin::hasThickFrame() const {
        if (::IsWindow(hWnd_)) {
            return ::GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_THICKFRAME;
        }
        return false;
    }

    bool WindowImplWin::setMouseCapture() {
        if (!::IsWindow(hWnd_)) {
            return false;
        }
        ::SetCapture(hWnd_);
        return true;
    }

    bool WindowImplWin::releaseMouseCapture() {
        if (!::IsWindow(hWnd_)) {
            return false;
        }
        if (::ReleaseCapture() == 0) {
            return false;
        }
        return true;
    }

    bool WindowImplWin::trackMouseHover(bool force) {
        return setMouseTrack(TME_HOVER, force);
    }

    bool WindowImplWin::setMouseTrack(DWORD flags, bool force) {
        if (!force) {
            if (!need_mouse_leave_track_ && flags == TME_LEAVE) {
                return false;
            }
            if (!need_mouse_hover_track_ && flags == TME_HOVER) {
                return false;
            }
        }

        if (!::IsWindow(hWnd_)) {
            return false;
        }

        UINT hover_time;
        // 返回毫秒
        if (::SystemParametersInfoW(SPI_GETMOUSEHOVERTIME, 0, &hover_time, 0) == 0) {
            hover_time = 400;
        }

        // 开启 Windows 的 WM_MOUSELEAVE, WM_MOUSEHOVER 事件支持
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = flags;
        tme.hwndTrack = hWnd_;
        tme.dwHoverTime = hover_time;
        if (::TrackMouseEvent(&tme) == 0) {
            return false;
        }

        if (flags & TME_LEAVE) {
            need_mouse_leave_track_ = false;
        }
        if (flags & TME_HOVER) {
            need_mouse_hover_track_ = false;
        }
        return true;
    }

    bool WindowImplWin::setWindowStyle(int style, bool ex, bool enabled) {
        if (!::IsWindow(hWnd_)) {
            return false;
        }

        // GetWindowLongPtr 调用成功也可能返回 0
        ::SetLastError(0);
        auto win_style = ::GetWindowLongPtr(hWnd_, ex ? GWL_EXSTYLE : GWL_STYLE);
        if (win_style == 0) {
            auto err = ::GetLastError();
            if (err != 0) {
                return false;
            }
        }

        if (enabled) {
            win_style |= style;
        } else {
            win_style &= ~style;
        }

        // SetWindowLongPtr 调用成功也可能返回 0
        ::SetLastError(0);
        auto prev_win_style = ::SetWindowLongPtr(hWnd_, ex ? GWL_EXSTYLE : GWL_STYLE, win_style);
        if (prev_win_style == 0) {
            auto err = ::GetLastError();
            if (err != 0) {
                return false;
            }
        }
        return true;
    }

    bool WindowImplWin::sendFrameChanged() {
        if (!::IsWindow(hWnd_)) {
            return false;
        }

        auto ret = ::SetWindowPos(hWnd_, nullptr, 0, 0, 0, 0,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOCOPYBITS |
            SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREPOSITION |
            SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);
        return ret != 0;
    }

    void WindowImplWin::forceResize() {
        if (isMinimized()) {
            return;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);

        bool handled;
        onSize(
            isMaximized() ? SIZE_MAXIMIZED : SIZE_RESTORED,
            MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top), &handled);
    }

    void WindowImplWin::convScreenToClient(Point* p) const {
        POINT raw_p { p->x(), p->y() };
        ::ScreenToClient(hWnd_, &raw_p);
        p->x(raw_p.x);
        p->y(raw_p.y);
    }

    void WindowImplWin::convClientToScreen(Point* p) const {
        POINT raw_p { p->x(), p->y() };
        ::ClientToScreen(hWnd_, &raw_p);
        p->x(raw_p.x);
        p->y(raw_p.y);
    }

    float WindowImplWin::scaleToNative(float val) const {
        if (Application::getOptions().is_auto_dpi_scale) {
            return val * getUserScale();
        }
        return val;
    }

    float WindowImplWin::scaleFromNative(float val) const {
        if (Application::getOptions().is_auto_dpi_scale) {
            return val / getUserScale();
        }
        return val;
    }

    void WindowImplWin::postMouseMove() {
        POINT pt;
        WPARAM wParam = 0;
        LPARAM lParam = 0;

        if (::GetKeyState(VK_LBUTTON) < 0) {
            wParam |= MK_LBUTTON;
        }
        if (::GetKeyState(VK_MBUTTON) < 0) {
            wParam |= MK_MBUTTON;
        }
        if (::GetKeyState(VK_RBUTTON) < 0) {
            wParam |= MK_RBUTTON;
        }
        if (::GetKeyState(VK_XBUTTON1) < 0) {
            wParam |= MK_XBUTTON1;
        }
        if (::GetKeyState(VK_XBUTTON2) < 0) {
            wParam |= MK_XBUTTON2;
        }
        if (::GetKeyState(VK_CONTROL) < 0) {
            wParam |= MK_CONTROL;
        }
        if (::GetKeyState(VK_SHIFT) < 0) {
            wParam |= MK_SHIFT;
        }

        if (::GetCursorPos(&pt) != 0) {
            ::ScreenToClient(hWnd_, &pt);
            lParam = MAKELPARAM(pt.x, pt.y);
        }

        ::PostMessageW(hWnd_, WM_MOUSEMOVE, wParam, lParam);
    }

    void WindowImplWin::setWindowRectShape(bool enabled) {
        if (enabled) {
            HRGN prev_rgn = ::CreateRectRgn(0, 0, 0, 0);
            int type = ::GetWindowRgn(hWnd_, prev_rgn);

            RECT win_rect{ 0, 0, 0, 0 };
            ::GetWindowRect(hWnd_, &win_rect);
            ::OffsetRect(&win_rect, -win_rect.left, -win_rect.top);
            HRGN new_rgn = ::CreateRectRgnIndirect(&win_rect);

            if (type == ERROR || !::EqualRgn(prev_rgn, new_rgn)) {
                ::SetWindowRgn(hWnd_, new_rgn, TRUE);
            } else {
                ::DeleteObject(new_rgn);
            }

            ::DeleteObject(prev_rgn);
        } else {
            ::SetWindowRgn(hWnd_, nullptr, TRUE);
        }
    }

    void WindowImplWin::createFrameIfNecessary() {
        if (!non_client_frame_) {
            if (init_params_.frame_type == WINDOW_FRAME_CUSTOM) {
                if (::IsWindows8OrGreater()) {
                    non_client_frame_.reset(new CustomNonClientFrame());
                } else {
                    non_client_frame_.reset(new CustomNonClientFrameWin7());
                }
            } else {
                non_client_frame_.reset(new NativeNonClientFrame());
            }
        }
    }

    void WindowImplWin::updateAppBarAwareRect() {
        int thickness = win::WinAppBar::getAutoHideBarThickness();

        auto display = DisplayWin::fromWindowImpl(this);
        if (!display->isValid()) {
            return;
        }

        Padding rect;
        auto edge = win::WinAppBar::findAutoHideEdge(display.get());
        switch (edge) {
        case win::WinAppBar::Bottom: rect.bottom(thickness); break;
        case win::WinAppBar::Left:   rect.start(thickness);   break;
        case win::WinAppBar::Right:  rect.end(thickness);  break;
        case win::WinAppBar::Top:    rect.top(thickness);    break;
        case win::WinAppBar::None:   break;
        }
        non_client_frame_->setExtraSpacingWhenMaximized(rect);
    }

    void WindowImplWin::setTabletPenServiceProperties(HWND hWnd) {
        /**
         * 参见
         * https://docs.microsoft.com/en-us/windows/win32/tablet/wm-tablet-querysystemgesturestatus-message
         */
        DWORD_PTR tablet_property =
            // disables press and hold (right-click) gesture
            TABLET_DISABLE_PRESSANDHOLD |
            // disables UI feedback on pen up (waves)
            TABLET_DISABLE_PENTAPFEEDBACK |
            // disables UI feedback on pen button down (circle)
            TABLET_DISABLE_PENBARRELFEEDBACK |
            // disables pen flicks (back, forward, drag down, drag up)
            TABLET_DISABLE_FLICKS;

        ATOM atom = ::GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);
        ::SetProp(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(tablet_property));
        ::GlobalDeleteAtom(atom);
    }

    void WindowImplWin::disableTouchFeedback(HWND hWnd) {
        if (::IsWindows8OrGreater()) {
            BOOL enabled = FALSE;
            win::UDSetWindowFeedbackSetting(hWnd, FEEDBACK_TOUCH_TAP, 0, sizeof(BOOL), &enabled);
            win::UDSetWindowFeedbackSetting(hWnd, FEEDBACK_TOUCH_PRESSANDHOLD, 0, sizeof(BOOL), &enabled);
            win::UDSetWindowFeedbackSetting(hWnd, FEEDBACK_TOUCH_RIGHTTAP, 0, sizeof(BOOL), &enabled);
        }
    }

    int WindowImplWin::getPointerTypeFromMouseMsg() {
        int pointer_type;
        auto info = ::GetMessageExtraInfo();
        if ((info & SIGNATURE_MASK) == MI_WP_SIGNATURE) {
            // Generated by Pen or Touch
            if (info & TOUCH_PEN_MASK) {
                // Touch
                pointer_type = InputEvent::PT_TOUCH;
            } else {
                // Pen
                pointer_type = InputEvent::PT_PEN;
            }
        } else {
            // Generated by Mouse
            pointer_type = InputEvent::PT_MOUSE;
        }
        return pointer_type;
    }

    bool WindowImplWin::isMouseLeaveTrackEnabled() const {
        return need_mouse_leave_track_;
    }

    bool WindowImplWin::isMouseHoverTrackEnabled() const {
        return need_mouse_hover_track_;
    }

    bool WindowImplWin::showTitlebarMenu() {
        if (!::IsWindow(hWnd_)) {
            return false;
        }

        HMENU sys_menu = ::GetSystemMenu(hWnd_, FALSE);
        if (!sys_menu) {
            return false;
        }

        auto mp_ptr = utl::MessagePump::getCurrent();
        auto pump = static_cast<utl::win::MessagePumpUIWin*>(mp_ptr.get());

        POINT pos;
        ::GetCursorPos(&pos);
        pump->setInDialogModalLoop(true);
        int cmd = ::TrackPopupMenuEx(
            sys_menu,
            ::GetSystemMetrics(SM_MENUDROPALIGNMENT) |
            TPM_TOPALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_HORIZONTAL,
            pos.x, pos.y, hWnd_, nullptr);
        pump->setInDialogModalLoop(false);
        if (cmd) {
            ::SendMessageW(hWnd_, WM_SYSCOMMAND, cmd, 0);
        }

        return true;
    }

    void WindowImplWin::onActivate(int param) {
        bool activate = false;

        switch (param) {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            activate = true;
            break;
        case WA_INACTIVE:
            activate = false;
            break;
        default:
            break;
        }

        delegate_->onActivate(activate);
    }

    void WindowImplWin::onMove(int x, int y) {
        prev_x_ = x_;
        x_ = x;
        prev_y_ = y_;
        y_ = y;

        ukive::scaleFromNative(this, &x);
        ukive::scaleFromNative(this, &y);

        delegate_->onMove(x, y);
    }

    void WindowImplWin::onResize(int param, int width, int height) {
        prev_width_ = width_;
        prev_height_ = height_;
        width_ = width;
        height_ = height;

        int type = -1;
        switch (param) {
        case SIZE_RESTORED:  type = WINDOW_RESIZE_RESTORED;  break;
        case SIZE_MINIMIZED: type = WINDOW_RESIZE_MINIMIZED; break;
        case SIZE_MAXIMIZED: type = WINDOW_RESIZE_MAXIMIZED; break;
        default: break;
        }

        ukive::scaleFromNative(this, &width);
        ukive::scaleFromNative(this, &height);

        delegate_->onResize(type, width, height);

        if (delegate_->onGetContext().getTranslucentType() & TRANS_LAYERED) {
            DirtyRegion region;
            region.setOne(getContentBounds());
            doDraw(region);
        }
    }

    bool WindowImplWin::onMoving(RECT* rect) {
        delegate_->onMoving();
        return false;
    }

    bool WindowImplWin::onResizing(WPARAM edge, RECT* rect) {
        Size new_size(
            rect->right - rect->left,
            rect->bottom - rect->top);
        ukive::scaleFromNative(this, &new_size);

        bool ret = delegate_->onResizing(&new_size);
        if (!ret) {
            return false;
        }

        ukive::scaleToNative(this, &new_size);

        switch (edge) {
        case WMSZ_TOP:
        case WMSZ_TOPLEFT:
            rect->top = rect->bottom - new_size.height();
            rect->left = rect->right - new_size.width();
            break;
        case WMSZ_BOTTOM:
        case WMSZ_BOTTOMLEFT:
            rect->bottom = rect->top + new_size.height();
            rect->left = rect->right - new_size.width();
            break;
        case WMSZ_LEFT:
            rect->left = rect->right - new_size.width();
            rect->top = rect->bottom - new_size.height();
            break;
        case WMSZ_RIGHT:
        case WMSZ_TOPRIGHT:
            rect->right = rect->left + new_size.width();
            rect->top = rect->bottom - new_size.height();
            break;
        case WMSZ_BOTTOMRIGHT:
            rect->bottom = rect->top + new_size.height();
            rect->right = rect->left + new_size.width();
            break;
        default:
            break;
        }
        return true;
    }

    bool WindowImplWin::onTouch(const TOUCHINPUT* inputs, int size) {
        bool consumed = false;
        for (int i = 0; i < size; ++i) {
            InputEvent ev;
            ev.setPointerType(InputEvent::PT_TOUCH);
            bool has_move = false;
            bool has_down_up = false;
            auto& input = inputs[i];
            if (input.dwFlags & TOUCHEVENTF_DOWN) {
                //DLOG(Log::INFO) << "TOUCH DOWN";

                ubassert(!has_down_up);
                ubassert(prev_ti_.find(input.dwID) == prev_ti_.end());
                has_down_up = true;
                ev.setEvent(InputEvent::EVT_DOWN);
                ev.setCurTouchId(utl::num_cast<int>(input.dwID));
                if (input.dwFlags & TOUCHEVENTF_PRIMARY) {
                    ev.setPrimaryTouchDown(true);
                }
                prev_ti_[input.dwID] = input;
            } else if (input.dwFlags & TOUCHEVENTF_UP) {
                //DLOG(Log::INFO) << "TOUCH UP";

                ubassert(!has_down_up);
                auto it = prev_ti_.find(input.dwID);
                ubassert(it != prev_ti_.end());
                has_down_up = true;
                ev.setEvent(InputEvent::EVT_UP);
                ev.setCurTouchId(utl::num_cast<int>(input.dwID));
                prev_ti_.erase(it);
            } else if (input.dwFlags & TOUCHEVENTF_MOVE) {
                if (!has_down_up && !has_move) {
                    auto it = prev_ti_.find(input.dwID);
                    ubassert(it != prev_ti_.end());
                    if (input.x != it->second.x || input.y != it->second.y) {
                        has_move = true;
                        ev.setEvent(InputEvent::EVT_MOVE);
                        ev.setCurTouchId(utl::num_cast<int>(input.dwID));
                    }
                }
                prev_ti_[input.dwID] = input;
            }

            if (has_down_up || has_move) {
                int tx = TOUCH_COORD_TO_PIXEL(input.x);
                int ty = TOUCH_COORD_TO_PIXEL(input.y);

                POINT pt = { tx, ty };
                ::ScreenToClient(hWnd_, &pt);

                if (/*(input.dwFlags & TOUCHEVENTF_PRIMARY) &&*/ (input.dwFlags & TOUCHEVENTF_UP)) {
                    is_prev_touched_ = true;
                    prev_touch_time_ = utl::TimeUtils::upTimeMillis();
                }

                ev.setRawX(pt.x);
                ev.setRawY(pt.y);
                ev.setX(pt.x);
                ev.setY(pt.y);

                consumed |= onInputEvent(&ev);
            }
        }

        return consumed;
    }

    bool WindowImplWin::onInputEvent(InputEvent* e) {
        // 追踪鼠标，以便产生 EVM_LEAVE_WIN 事件。
        if (e->getEvent() == InputEvent::EVM_LEAVE_WIN) {
            need_mouse_leave_track_ = true;
            need_mouse_hover_track_ = true;
        } else if (e->getEvent() == InputEvent::EVM_HOVER) {
            need_mouse_hover_track_ = true;
        } else if (e->getEvent() == InputEvent::EVM_MOVE) {
            setMouseTrack(TME_LEAVE, false);
        }

        e->transformInputPos(
            [this](InputEvent::InputPos* pos)
        {
            ukive::scaleFromNative(this, &pos->pos.x());
            ukive::scaleFromNative(this, &pos->pos.y());
            ukive::scaleFromNative(this, &pos->raw.x());
            ukive::scaleFromNative(this, &pos->raw.y());
        });

        bool ret = delegate_->onInputEvent(e);

        if (e->getEvent() == InputEvent::EVK_UP &&
            e->getKeyboardKey() == Keyboard::KEY_ESC)
        {
            if (close_methods_ & WINDOW_CLOSE_BY_ESC) {
                close();
                ret = true;
            }
        }

        return ret;
    }

    void WindowImplWin::onDpiChanged(int dpi_x, int dpi_y) {
        auto context = delegate_->onGetContext();
        if (Application::getOptions().is_auto_dpi_scale) {
            context.setScale(1);
            context.setAutoScale(float(dpi_x) / kDefaultDpi);
        } else {
            context.setAutoScale(1);
            context.setScale(float(dpi_x) / kDefaultDpi);
        }
        delegate_->onUpdateContext(Context::DPI_CHANGED);

        Application::getGraphicDeviceManager()->notifyDPIChanged(float(dpi_x), float(dpi_y));
    }

    void WindowImplWin::onStyleChanged(bool normal, bool ext, const STYLESTRUCT* ss) {
        /**
         * 暂不同步外部的改变
         */
        return;
        /*if (normal) {
            is_resizable_ = ss->styleNew & WS_THICKFRAME;
            is_maximizable_ = ss->styleNew & WS_MAXIMIZEBOX;
            is_minimizable_ = ss->styleNew & WS_MINIMIZEBOX;
        }
        if (ext) {
            is_show_in_task_bar_ = !(ss->styleNew & WS_EX_TOOLWINDOW);
            is_ignore_mouse_events_ = ss->styleNew & WS_EX_TRANSPARENT;
        }*/
    }

    bool WindowImplWin::onDataCopy(ULONG_PTR id, DWORD size, void* data) {
        return false;
    }

    LRESULT WindowImplWin::onNCCreate(WPARAM wParam, LPARAM lParam, bool* handled) {
        static bool is_win8 = ::IsWindows8OrGreater();
        if (is_win8) {
            display_power_notify_ = ::RegisterPowerSettingNotification(
                hWnd_, &GUID_CONSOLE_DISPLAY_STATE, DEVICE_NOTIFY_WINDOW_HANDLE);
        } else {
            display_power_notify_ = ::RegisterPowerSettingNotification(
                hWnd_, &GUID_MONITOR_POWER_ON, DEVICE_NOTIFY_WINDOW_HANDLE);
        }
        if (!display_power_notify_) {
            LOG(Log::WARNING) << "Failed to register power notification: " << GetLastError();
        }

        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onNcCreate(this, handled);

        delegate_->onCreate();
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onCreate(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }

    LRESULT WindowImplWin::onNCDrawClassic1(WPARAM wParam, LPARAM lParam, bool* handled) {
        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onInterceptDrawClassic(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onNCDrawClassic2(WPARAM wParam, LPARAM lParam, bool* handled) {
        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onInterceptDrawClassic(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onNCPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onNcPaint(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (::GetUpdateRect(hWnd_, nullptr, FALSE) == 0) {
            return 0;
        }

        PAINTSTRUCT ps;
        ::BeginPaint(hWnd_, &ps);

        Rect rect(
            ps.rcPaint.left, ps.rcPaint.top,
            ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);

        ukive::scaleFromNative(this, &rect);

        DirtyRegion region;
        region.setOne(rect);
        delegate_->onDraw(region);

        ::EndPaint(hWnd_, &ps);
        *handled = true;
        return 0;
    }

    LRESULT WindowImplWin::onNCActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onNcActivate(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onNCHitTest(WPARAM wParam, LPARAM lParam, bool* handled) {
        createFrameIfNecessary();

        POINT point;
        bool pass_to_window = false;
        auto nc_result = non_client_frame_->onNcHitTest(wParam, lParam, handled, &pass_to_window, &point);
        if (*handled && pass_to_window) {
            Point pt{ point.x, point.y };
            ukive::scaleFromNative(this, &pt);

            int win_hp;
            auto hit_point = delegate_->onNCHitTest(pt.x(), pt.y());

            switch (hit_point) {
            case HitPoint::TOP_LEFT:
            case HitPoint::TOP:
            case HitPoint::TOP_RIGHT:
            case HitPoint::LEFT:
            case HitPoint::RIGHT:
            case HitPoint::BOTTOM_LEFT:
            case HitPoint::BOTTOM:
            case HitPoint::BOTTOM_RIGHT:
                if (!hasThickFrame() || !is_resizable_) {
                    hit_point = HitPoint::CLIENT;
                }
                break;
            case HitPoint::CAPTION:
                if (!hasCaptain()) {
                    hit_point = HitPoint::CLIENT;
                }
                break;
            default: break;
            }

            switch (hit_point) {
            case HitPoint::TOP_LEFT:     win_hp = HTTOPLEFT;     break;
            case HitPoint::TOP:          win_hp = HTTOP;         break;
            case HitPoint::TOP_RIGHT:    win_hp = HTTOPRIGHT;    break;
            case HitPoint::LEFT:         win_hp = HTLEFT;        break;
            case HitPoint::RIGHT:        win_hp = HTRIGHT;       break;
            case HitPoint::BOTTOM_LEFT:  win_hp = HTBOTTOMLEFT;  break;
            case HitPoint::BOTTOM:       win_hp = HTBOTTOM;      break;
            case HitPoint::BOTTOM_RIGHT: win_hp = HTBOTTOMRIGHT; break;
            case HitPoint::CAPTION:      win_hp = HTCAPTION;     break;
            case HitPoint::SYS_MENU:     win_hp = HTSYSMENU;     break;
            case HitPoint::MIN_BUTTON:   win_hp = HTMINBUTTON;   break;
            case HitPoint::MAX_BUTTON:   win_hp = HTMAXBUTTON;   break;
            case HitPoint::CLOSE_BUTTON: win_hp = HTCLOSE;       break;
            default:                     win_hp = HTCLIENT;      break;
            }

            nc_result = win_hp;
        }
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onNCCalCSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (is_first_nccalc_) {
            /**
             * 第一次先走默认，让窗口记住一些初始值。
             * 当前发现这里影响在窗口的系统菜单中选择“移动”时鼠标自动移动的位置。
             */
            is_first_nccalc_ = false;
            return 0;
        }

        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onNcCalSize(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onMouseRange(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled) {
        LRESULT nc_result = 0;

        switch (uMsg) {
        case WM_NCLBUTTONDOWN:
            createFrameIfNecessary();
            nc_result = non_client_frame_->onNcLButtonDown(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }
            break;

        case WM_NCLBUTTONUP:
            createFrameIfNecessary();
            nc_result = non_client_frame_->onNcLButtonUp(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }
            break;

        case WM_NCRBUTTONDOWN:
            createFrameIfNecessary();
            nc_result = non_client_frame_->onNcRButtonDown(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }
            break;

        case WM_NCRBUTTONUP:
            createFrameIfNecessary();
            nc_result = non_client_frame_->onNcRButtonUp(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }
            break;

        case WM_NCMOUSEMOVE:
            *handled = true;
            return 0;

        case WM_LBUTTONDOWN:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            createFrameIfNecessary();
            nc_result = non_client_frame_->OnLButtonUp(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_RBUTTONDOWN:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_RBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MBUTTONDOWN:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_XBUTTONDOWN:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(
                (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ?
                InputEvent::MK_XBUTTON_1 : InputEvent::MK_XBUTTON_2);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_XBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(
                (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ?
                InputEvent::MK_XBUTTON_1 : InputEvent::MK_XBUTTON_2);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                prev_touch_x_ = GET_X_LPARAM(lParam);
                prev_touch_y_ = GET_Y_LPARAM(lParam);
                return 0;
            }

            // 有时，在某些具有触屏的设备上启动程序时，以及接收到 WM_TOUCH 后，
            // Windows 会随机触发 WM_MOUSEMOVE 或 WM_LBUTTONUP 事件，而这些
            // 事件会被 GetMessageExtraInfo() 判定为鼠标事件（尽管这些事件是由操作触屏引发的）。
            // 以下网址中包含有相关讨论和解决方法：
            // https://social.msdn.microsoft.com/Forums/en-US/1b7217bb-1e60-4e00-83c9-193c7f88c249
            if (is_prev_touched_) {
                if (utl::TimeUtils::upTimeMillis() - prev_touch_time_ <= 1000) {
                    if (GET_X_LPARAM(lParam) == prev_touch_x_ && GET_Y_LPARAM(lParam) == prev_touch_y_) {
                        is_prev_touched_ = false;
                        return 0;
                    }
                } else {
                    is_prev_touched_ = false;
                }
            }

            createFrameIfNecessary();
            nc_result = non_client_frame_->onMouseMove(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_MOVE);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            int wheel = GET_WHEEL_DELTA_WPARAM(wParam);
            // 猜测滚动粒度，目前没找到更好的方法。
            InputEvent::WheelGranularity granularity
                = (std::abs(wheel) % WHEEL_DELTA) == 0 ? InputEvent::WG_DELTA : InputEvent::WG_HIGH_PRECISE;

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_WHEEL);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setWheelValue(GET_WHEEL_DELTA_WPARAM(wParam), granularity);

            ::POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ::ScreenToClient(hWnd_, &pt);

            ev.setX(pt.x);
            ev.setY(pt.y);
            ev.setRawX(pt.x);
            ev.setRawY(pt.y);
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        default:
            break;
        }
        return 0;
    }

    LRESULT WindowImplWin::onMouseHover(WPARAM wParam, LPARAM lParam, bool* handled) {
        int p_type = getPointerTypeFromMouseMsg();
        if (p_type != InputEvent::PT_MOUSE &&
            p_type != InputEvent::PT_PEN)
        {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVM_HOVER);
        ev.setPointerType(InputEvent::PT_MOUSE);
        ev.setX(GET_X_LPARAM(lParam));
        ev.setY(GET_Y_LPARAM(lParam));
        ev.setRawX(GET_X_LPARAM(lParam));
        ev.setRawY(GET_Y_LPARAM(lParam));
        if (onInputEvent(&ev)) {
            *handled = true;
            return 0;
        }

        return 0;
    }

    LRESULT WindowImplWin::onMouseLeave(WPARAM wParam, LPARAM lParam, bool* handled) {
        int p_type = getPointerTypeFromMouseMsg();
        if (p_type != InputEvent::PT_MOUSE &&
            p_type != InputEvent::PT_PEN)
        {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVM_LEAVE_WIN);
        ev.setPointerType(InputEvent::PT_MOUSE);
        if (onInputEvent(&ev)) {
            *handled = true;
            return 0;
        }

        return 0;
    }

    LRESULT WindowImplWin::onClose(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (!delegate_->onClose()) {
            *handled = true;
        }
        return 0;
    }

    LRESULT WindowImplWin::onDestroy(WPARAM wParam, LPARAM lParam, bool* handled) {
        is_created_ = false;
        is_showing_ = false;
        delegate_->onDestroy();

        *handled = true;
        return 0;
    }

    LRESULT WindowImplWin::onNCDestroy(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto nc_result = non_client_frame_->onNcDestroy(handled);
        if (*handled) {
            return nc_result;
        }

        if (display_power_notify_) {
            ::UnregisterPowerSettingNotification(display_power_notify_);
            display_power_notify_ = nullptr;
        }

        delegate_->onDestroyed();
        return 0;
    }

    LRESULT WindowImplWin::onShowWindow(WPARAM wParam, LPARAM lParam, bool* handled) {
        delegate_->onShow(static_cast<BOOL>(wParam) == TRUE ? true : false);
        return 0;
    }

    LRESULT WindowImplWin::onActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        onActivate(LOWORD(wParam));
        return 0;
    }

    LRESULT WindowImplWin::onDPIChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        int new_dpi_x = LOWORD(wParam);
        int new_dpi_y = HIWORD(wParam);

        auto nw_rect = reinterpret_cast<RECT*>(lParam);
        ::SetWindowPos(hWnd_,
            nullptr,
            nw_rect->left,
            nw_rect->top,
            nw_rect->right - nw_rect->left,
            nw_rect->bottom - nw_rect->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        onDpiChanged(new_dpi_x, new_dpi_y);
        return 0;
    }

    LRESULT WindowImplWin::onStyleChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        onStyleChanged(
            wParam & GWL_STYLE, wParam & GWL_EXSTYLE,
            reinterpret_cast<const STYLESTRUCT*>(lParam));
        return 0;
    }

    LRESULT WindowImplWin::onCopyData(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto cds = reinterpret_cast<PCOPYDATASTRUCT>(lParam);
        if (!cds) {
            return 0;
        }

        if (onDataCopy(cds->dwData, cds->cbData, cds->lpData)) {
            *handled = true;
            return TRUE;
        }

        return FALSE;
    }

    LRESULT WindowImplWin::onEraseBkgnd(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return 1;
    }

    LRESULT WindowImplWin::onSetCursor(WPARAM wParam, LPARAM lParam, bool* handled) {
        int hit_code = LOWORD(lParam);

        if (hit_code == HTCLIENT) {
            *handled = true;
            setCurrentCursor(cursor_);
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImplWin::onSetFocus(WPARAM wParam, LPARAM lParam, bool* handled) {
        delegate_->onSetFocus();
        return 0;
    }

    LRESULT WindowImplWin::onKillFocus(WPARAM wParam, LPARAM lParam, bool* handled) {
        delegate_->onKillFocus();
        return 0;
    }

    LRESULT WindowImplWin::onSetText(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto text = reinterpret_cast<wchar_t*>(lParam);
        delegate_->onSetText(std::u16string(text, text + std::char_traits<wchar_t>::length(text)));
        return 0;
    }

    LRESULT WindowImplWin::onSetIcon(WPARAM wParam, LPARAM lParam, bool* handled) {
        delegate_->onSetIcon();
        return 0;
    }

    LRESULT WindowImplWin::onMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        int x_px = LOWORD(lParam);
        int y_px = HIWORD(lParam);
        onMove(x_px, y_px);
        return 0;
    }

    LRESULT WindowImplWin::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        if ((delegate_->onGetContext().getTranslucentType() & TRANS_LAYERED) ||
            !win::isAeroEnabled())
        {
            setWindowRectShape(true);
        }

        RECT w_rect;
        ::GetWindowRect(hWnd_, &w_rect);

        RECT c_rect;
        ::GetClientRect(hWnd_, &c_rect);

        int width_px = w_rect.right - w_rect.left;
        int height_px = w_rect.bottom - w_rect.top;

        onResize(int(wParam), width_px, height_px);
        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onSize(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImplWin::onMoving(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto raw_rect = reinterpret_cast<RECT*>(lParam);
        *handled = onMoving(raw_rect);
        if (*handled) {
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImplWin::onSizing(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto raw_rect = reinterpret_cast<RECT*>(lParam);
        *handled = onResizing(wParam, raw_rect);
        if (*handled) {
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImplWin::onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled) {
        createFrameIfNecessary();
        return non_client_frame_->onGetMinMaxInfo(wParam, lParam, handled);
    }

    LRESULT WindowImplWin::onKeyDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        int key;
        if (!Keyboard::mapKey(int(wParam), &key)) {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVK_DOWN);
        ev.setPointerType(InputEvent::PT_KEYBOARD);
        ev.setKeyboardKey(key, (lParam & 0x0000ffff) > 1);

        if (onInputEvent(&ev)) {
            *handled = true;
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImplWin::onKeyUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        int key;
        if (!Keyboard::mapKey(int(wParam), &key)) {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVK_UP);
        ev.setPointerType(InputEvent::PT_KEYBOARD);
        ev.setKeyboardKey(key, (lParam & 0x0000ffff) > 1);

        if (onInputEvent(&ev)) {
            *handled = true;
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImplWin::onSysKeyDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImplWin::onSysKeyUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImplWin::onSysCommand(WPARAM wParam, LPARAM lParam, bool* handled) {
        switch (wParam & 0xFFF0) {
        case SC_CLOSE:
        {
            if (!isClosable()) {
                *handled = true;
                return TRUE;
            }

            // ALT + F4
            if (!(close_methods_ & WINDOW_CLOSE_BY_MENU)) {
                *handled = true;
                return TRUE;
            }
            break;
        }

        case SC_MAXIMIZE:
            if (!isMaximizable()) {
                *handled = true;
                return TRUE;
            }
            break;

        case SC_MINIMIZE:
            if (!isMinimizable()) {
                *handled = true;
                return TRUE;
            }
            break;

        case SC_SIZE:
        {
            if (!isResizable()) {
                *handled = true;
                return TRUE;
            }

            /**
             * 调整窗口大小或位置时，DefWindowProc 内部会开一个消息循环陷入其中，我们自己的消息队列的
             * 消息在此期间将无法得到处理。为了能处理我们自己的消息，我们在 MessagePumpUIWin 里安装
             * 了消息钩子，在钩子的处理函数中处理我们自己的消息。这就相当于将我们自己的消息处理逻辑
             * “注入”到内部的消息循环中。
             */
            *handled = true;
            auto mp_ptr = utl::MessagePump::getCurrent();
            auto pump = static_cast<utl::win::MessagePumpUIWin*>(mp_ptr.get());

            pump->setInSizeModalLoop(true);
            auto ret = ::DefWindowProcW(hWnd_, WM_SYSCOMMAND, wParam, lParam);
            pump->setInSizeModalLoop(false);
            return ret;
        }

        case SC_RESTORE:
            if ((!isMaximized() && !isMinimized()) || isFullscreen()) {
                *handled = true;
                return TRUE;
            }
            break;

        case SC_MOVE:
        {
            if (isMinimized() || isFullscreen()) {
                *handled = true;
                return TRUE;
            }

            // 参见上方 SC_SIZE 里的注释
            *handled = true;
            auto mp_ptr = utl::MessagePump::getCurrent();
            auto pump = static_cast<utl::win::MessagePumpUIWin*>(mp_ptr.get());

            pump->setInMoveModalLoop(true);
            /**
             * 鼠标左键在标题栏上按下时，在 DefWindowProc 中进入嵌套消息循环前会先阻塞一段时间。
             * 为消除这段时间，给窗口发个 WM_MOUSEMOVE 消息，可以立刻进入嵌套消息循环。
             */
            //postMouseMove();
            auto ret = ::DefWindowProcW(hWnd_, WM_SYSCOMMAND, wParam, lParam);
            pump->setInMoveModalLoop(false);
            return ret;
        }

        default:
            break;
        }

        return 0;
    }

    LRESULT WindowImplWin::onCommand(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImplWin::onInitMenu(WPARAM wParam, LPARAM lParam, bool* handled) {
        bool is_restorable = (isMaximized() || isMinimized()) && !isFullscreen();
        bool is_movable = !isMaximized() && !isMinimized() && !isFullscreen();

        HMENU menu = reinterpret_cast<HMENU>(wParam);
        ::EnableMenuItem(menu, SC_RESTORE, MF_BYCOMMAND | (is_restorable ? MF_ENABLED : MF_GRAYED));
        ::EnableMenuItem(menu, SC_SIZE, MF_BYCOMMAND | (isResizable() ? MF_ENABLED : MF_GRAYED));
        ::EnableMenuItem(menu, SC_MOVE, MF_BYCOMMAND | (is_movable ? MF_ENABLED : MF_GRAYED));
        ::EnableMenuItem(menu, SC_MAXIMIZE, MF_BYCOMMAND | (isMaximizable() ? MF_ENABLED : MF_GRAYED));
        ::EnableMenuItem(menu, SC_MINIMIZE, MF_BYCOMMAND | (isMinimizable() ? MF_ENABLED : MF_GRAYED));
        ::EnableMenuItem(menu, SC_CLOSE, MF_BYCOMMAND | (isClosable() ? MF_ENABLED : MF_GRAYED));
        return 0;
    }

    LRESULT WindowImplWin::onInitMenuPopup(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImplWin::onChar(WPARAM wParam, LPARAM lParam, bool* handled) {
        std::u16string str;
        str.push_back(char16_t(wParam));

        if (Keyboard::filterChars(str)) {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVK_CHARS);
        ev.setPointerType(InputEvent::PT_KEYBOARD);
        ev.setKeyboardChars(str, (lParam & 0x0000ffff) > 1);

        if (onInputEvent(&ev)) {
            *handled = true;
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImplWin::onUniChar(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImplWin::onTouch(WPARAM wParam, LPARAM lParam, bool* handled) {
        UINT input_count = LOWORD(wParam);
        if (input_count == 0) {
            return 0;
        }

        if (input_count > ti_cache_.size) {
            ti_cache_.cache.reset(new TOUCHINPUT[input_count]);
            ti_cache_.size = input_count;
        }

        if (::GetTouchInputInfo(
            reinterpret_cast<HTOUCHINPUT>(lParam),
            input_count,
            ti_cache_.cache.get(),
            sizeof(TOUCHINPUT)))
        {
            *handled = onTouch(ti_cache_.cache.get(), utl::num_cast<int>(input_count));
            ::CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));

            if (*handled) {
                return 0;
            }
        }
        return 0;
    }

    LRESULT WindowImplWin::onDisplayChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        static_cast<DisplayManagerWin*>(
            Application::getDisplayManager())->notifyChanged(hWnd_);

        *handled = false;
        display_ = DisplayWin::fromWindowImpl(this);

        ::InvalidateRect(hWnd_, nullptr, FALSE);
        ::UpdateWindow(hWnd_);
        return 0;
    }

    LRESULT WindowImplWin::onDwmCompositionChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        setWindowRectShape(!win::isAeroEnabled());

        createFrameIfNecessary();
        auto nc_result = non_client_frame_->onDwmCompositionChanged(handled);
        if (*handled) {
            return nc_result;
        }

        auto context = delegate_->onGetContext();

        auto& config = context.getCurrentThemeConfig();
        config.type = ThemeConfig::COLOR_EXISTANCE;
        config.has_color = win::isAeroEnabled();

        if (config.has_color) {
            if (context.getTranslucentType() & TRANS_SYSTEM) {
                setTranslucent(config.transparency_enabled);
            } else {
                setTranslucent(true);
            }
        }

        forceResize();

        delegate_->onUpdateContext(Context::THEME_CHANGED);
        return 0;
    }

    LRESULT WindowImplWin::onDwmColorizationColorChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        DWORD color = DWORD(wParam);
        //BOOL opaque_blend = BOOL(lParam);

        auto context = delegate_->onGetContext();

        auto& config = context.getCurrentThemeConfig();
        config.type = ThemeConfig::COLOR_CHANGED;
        config.primary_color = Color::ofARGB(color);

        delegate_->onUpdateContext(Context::THEME_CHANGED);
        return 0;
    }

    LRESULT WindowImplWin::onSettingChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        switch (wParam) {
        case 0:
        {
            LPWSTR str = reinterpret_cast<LPWSTR>(lParam);
            if (str && std::wcscmp(str, L"ImmersiveColorSet") == 0) {
                RegManager::SysThemeConfig info;
                if (RegManager::getSysThemeConfig(&info)) {
                    auto context = delegate_->onGetContext();

                    if (context.getTranslucentType() & TRANS_SYSTEM) {
                        setTranslucent(info.transparency_enabled);
                    }

                    auto& config = context.getCurrentThemeConfig();
                    config.type = ThemeConfig::CONFIG_CHANGED;
                    config.light_theme = info.apps_use_light_theme;
                    config.transparency_enabled = info.transparency_enabled;

                    delegate_->onUpdateContext(Context::THEME_CHANGED);
                }
            }
            break;
        }

        case SPI_SETWORKAREA:
        {
            if (!non_client_frame_) {
                break;
            }
            updateAppBarAwareRect();
            break;
        }
        default:
            break;
        }

        return 0;
    }

    LRESULT WindowImplWin::onThemeChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        //forceResize();
        return 0;
    }

    LRESULT WindowImplWin::onWindowPosChanging(WPARAM wParam, LPARAM lParam, bool* handled) {
        /*auto win_pos = reinterpret_cast<WINDOWPOS*>(lParam);
        win_pos->x = 0;
        win_pos->y = 0;
        win_pos->cx = 3840;
        win_pos->cy = 2160;
        *handled = true;*/
        return 0;
    }

    LRESULT WindowImplWin::onWindowPosChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto win_pos = reinterpret_cast<WINDOWPOS*>(lParam);
        if (win_pos->flags & SWP_FRAMECHANGED) {
        }

        if (!(win_pos->flags & SWP_NOZORDER) && ::IsWindow(hWnd_)) {
            is_keep_on_top_ = ::GetWindowLongPtr(hWnd_, GWL_EXSTYLE) & WS_EX_TOPMOST;
        }

        auto display = DisplayWin::fromWindowImpl(this);
        if (!display_ || !display_->isSame(display.get())) {
            display_ = display;
        }

        return 0;
    }

    LRESULT WindowImplWin::onPowerBroadcast(WPARAM wParam, LPARAM lParam, bool* handled) {
        switch (wParam) {
        case PBT_APMPOWERSTATUSCHANGE:
        {
            SYSTEM_POWER_STATUS status;
            if (::GetSystemPowerStatus(&status) == 0) {
                LOG(Log::WARNING) << "Failed to get system power status: " << ::GetLastError();
                break;
            }
            /*LOG(Log::INFO) << "Power status: [AC-" << status.ACLineStatus
                << ", Battery-" << status.BatteryFlag
                << ", Battery Life<" << status.BatteryLifePercent << "%, "
                << status.SystemStatusFlag << ", " << status.BatteryLifeTime << ", " << status.BatteryFullLifeTime
                << ">]";*/
            break;
        }
        case PBT_APMRESUMEAUTOMATIC:
            //LOG(Log::INFO) << "Power event: resume automatic";
            break;
        case PBT_APMRESUMESUSPEND:
            //LOG(Log::INFO) << "Power event: resume suspend";
            break;
        case PBT_APMSUSPEND:
            //LOG(Log::INFO) << "Power event: suspend";
            break;
        case PBT_POWERSETTINGCHANGE:
        {
            auto settings = reinterpret_cast<POWERBROADCAST_SETTING*>(lParam);
            if (settings->PowerSetting == GUID_CONSOLE_DISPLAY_STATE ||
                settings->PowerSetting == GUID_MONITOR_POWER_ON)
            {
                ubassert(settings->DataLength == 4);
                auto value = *reinterpret_cast<DWORD*>(settings->Data);
                //LOG(Log::INFO) << "Power settings: Monitor[" << value << "]";

                auto vsync_provider = static_cast<VSyncProviderWin*>(Application::getVSyncProvider());
                vsync_provider->setPrimaryMonitorStatus(value != 0);
            }
            break;
        }
        default:
            break;
        }
        return 0;
    }

    LRESULT WindowImplWin::processDWMProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP)
    {
        LRESULT ret = 0;
        bool call_dwp = !::DwmDefWindowProc(hWnd, message, wParam, lParam, &ret);

        if (message == WM_CREATE) {
            RECT rect;
            ::GetWindowRect(hWnd, &rect);

            // 通知窗口边框变化，以便尽早应用新的非客户区大小
            ::SetWindowPos(hWnd,
                nullptr,
                rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top,
                SWP_FRAMECHANGED);

            call_dwp = true;
            ret = 0;
        }

        if (message == WM_ACTIVATE) {
            createFrameIfNecessary();
            non_client_frame_->onActivateAfterDwm();

            auto context = delegate_->onGetContext();
            if (context.getTranslucentType() & TRANS_SYSTEM) {
                setTranslucent(
                    context.getCurrentThemeConfig().has_color &&
                    context.getCurrentThemeConfig().transparency_enabled);
            } else {
                setTranslucent(context.getCurrentThemeConfig().has_color);
            }

            call_dwp = true;
            ret = 0;
        }

        if (message == WM_PAINT) {
            call_dwp = true;
            ret = 0;
        }

        *pfCallDWP = call_dwp;
        return ret;
    }

    LRESULT WindowImplWin::processWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled) {
        WINDOW_MSG_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, onMouseRange);
        WINDOW_MSG_RANGE_HANDLER(WM_NCMOUSEFIRST, WM_NCMOUSELAST, onMouseRange);

        WINDOW_MSG_HANDLER(WM_MOUSEHOVER, onMouseHover);
        WINDOW_MSG_HANDLER(WM_MOUSELEAVE, onMouseLeave);

        WINDOW_MSG_HANDLER(WM_NCCREATE, onNCCreate);
        WINDOW_MSG_HANDLER(WM_CREATE, onCreate);
        WINDOW_MSG_HANDLER(WM_NCDRAWCLASSIC1, onNCDrawClassic1);
        WINDOW_MSG_HANDLER(WM_NCDRAWCLASSIC2, onNCDrawClassic2);
        WINDOW_MSG_HANDLER(WM_NCPAINT, onNCPaint);
        WINDOW_MSG_HANDLER(WM_PAINT, onPaint);
        WINDOW_MSG_HANDLER(WM_NCACTIVATE, onNCActivate);
        WINDOW_MSG_HANDLER(WM_NCHITTEST, onNCHitTest);
        WINDOW_MSG_HANDLER(WM_NCCALCSIZE, onNCCalCSize);
        WINDOW_MSG_HANDLER(WM_CLOSE, onClose);
        WINDOW_MSG_HANDLER(WM_DESTROY, onDestroy);
        WINDOW_MSG_HANDLER(WM_NCDESTROY, onNCDestroy);
        WINDOW_MSG_HANDLER(WM_SHOWWINDOW, onShowWindow);
        WINDOW_MSG_HANDLER(WM_ACTIVATE, onActivate);
        WINDOW_MSG_HANDLER(WM_DPICHANGED, onDPIChanged);
        WINDOW_MSG_HANDLER(WM_STYLECHANGED, onStyleChanged);
        WINDOW_MSG_HANDLER(WM_COPYDATA, onCopyData);
        WINDOW_MSG_HANDLER(WM_ERASEBKGND, onEraseBkgnd);
        WINDOW_MSG_HANDLER(WM_SETCURSOR, onSetCursor);
        WINDOW_MSG_HANDLER(WM_SETFOCUS, onSetFocus);
        WINDOW_MSG_HANDLER(WM_KILLFOCUS, onKillFocus);
        WINDOW_MSG_HANDLER(WM_SETTEXT, onSetText);
        WINDOW_MSG_HANDLER(WM_SETICON, onSetIcon);
        WINDOW_MSG_HANDLER(WM_MOVE, onMove);
        WINDOW_MSG_HANDLER(WM_SIZE, onSize);
        WINDOW_MSG_HANDLER(WM_MOVING, onMoving);
        WINDOW_MSG_HANDLER(WM_SIZING, onSizing);
        WINDOW_MSG_HANDLER(WM_GETMINMAXINFO, onGetMinMaxInfo);
        WINDOW_MSG_HANDLER(WM_KEYDOWN, onKeyDown);
        WINDOW_MSG_HANDLER(WM_KEYUP, onKeyUp);
        WINDOW_MSG_HANDLER(WM_SYSKEYDOWN, onSysKeyDown);
        WINDOW_MSG_HANDLER(WM_SYSKEYUP, onSysKeyUp);
        WINDOW_MSG_HANDLER(WM_SYSCOMMAND, onSysCommand);
        WINDOW_MSG_HANDLER(WM_COMMAND, onCommand);
        WINDOW_MSG_HANDLER(WM_INITMENU, onInitMenu);
        WINDOW_MSG_HANDLER(WM_INITMENUPOPUP, onInitMenuPopup);
        WINDOW_MSG_HANDLER(WM_CHAR, onChar);
        WINDOW_MSG_HANDLER(WM_UNICHAR, onUniChar);
        WINDOW_MSG_HANDLER(WM_TOUCH, onTouch);
        WINDOW_MSG_HANDLER(WM_DISPLAYCHANGE, onDisplayChanged);
        WINDOW_MSG_HANDLER(WM_DWMCOMPOSITIONCHANGED, onDwmCompositionChanged);
        WINDOW_MSG_HANDLER(WM_DWMCOLORIZATIONCOLORCHANGED, onDwmColorizationColorChanged);
        WINDOW_MSG_HANDLER(WM_SETTINGCHANGE, onSettingChanged);
        WINDOW_MSG_HANDLER(WM_THEMECHANGED, onThemeChanged);
        WINDOW_MSG_HANDLER(WM_WINDOWPOSCHANGING, onWindowPosChanging);
        WINDOW_MSG_HANDLER(WM_WINDOWPOSCHANGED, onWindowPosChanged);
        WINDOW_MSG_HANDLER(WM_POWERBROADCAST, onPowerBroadcast);

        return 0;
    }

    LRESULT CALLBACK WindowImplWin::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        WindowImplWin* window = nullptr;
        if (uMsg == WM_NCCREATE) {
            disableTouchFeedback(hWnd);
            setTabletPenServiceProperties(hWnd);
            /**
             * 使用手掌检测 TWF_WANTPALM 会带来 100ms 左右的延时。
             * 参见 https://docs.microsoft.com/zh-cn/windows/win32/wintouch/troubleshooting-applications
             */
            if (::RegisterTouchWindow(hWnd, TWF_FINETOUCH) == 0) {
                LOG(Log::WARNING) << "Failed to register touch window: " << GetLastError();
            }

            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            window = static_cast<WindowImplWin*>(cs->lpCreateParams);
            if (!window) {
                LOG(Log::FATAL) << "null window creating param.";
                return FALSE;
            }

            window->hWnd_ = hWnd;
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }

        if (uMsg == WM_NCDESTROY) {
            if (::UnregisterTouchWindow(hWnd) == 0) {
                LOG(Log::WARNING) << "Failed to unregister touch window: " << GetLastError();
            }
        }

        if (!window) {
            window = reinterpret_cast<WindowImplWin*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (window) {
            bool call_dwp = true;
            if (win::isAeroEnabled()) {
                LRESULT lRet = window->processDWMProc(hWnd, uMsg, wParam, lParam, &call_dwp);
                if (!call_dwp) {
                    return lRet;
                }
            }

            if (call_dwp) {
                bool handled = false;
                auto result = window->processWindowMessage(uMsg, wParam, lParam, &handled);
                if (handled) {
                    return result;
                }
            }
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    bool WindowImplWin::setLayered(bool enabled) {
        if (!setWindowStyle(WS_EX_LAYERED, true, enabled)) {
            return false;
        }
        non_client_frame_->onTranslucentChanged(enabled);
        sendFrameChanged();
        return true;
    }

    bool WindowImplWin::setBlurBehind(bool enabled) {
        static bool is_win8_or_greater = IsWindows8OrGreater();
        static bool is_win10_or_greater = IsWindows10OrGreater();

        if (is_win10_or_greater) {
            return setBlurBehindOnWin10(enabled);
        } else if (!is_win8_or_greater) {
            return setBlurBehindOnWin7(enabled);
        }
        return false;
    }

    bool WindowImplWin::setTransparent(bool enabled) {
        // Create and populate the Blur Behind structure
        DWM_BLURBEHIND bb = { 0 };

        if (!blur_rgn_ && enabled) {
            blur_rgn_ = ::CreateRectRgn(0, 0, -1, -1);
            if (!blur_rgn_) {
                return false;
            }
        }

        if (enabled) {
            // Enable Blur Behind and apply to the entire client area
            bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
            bb.fEnable = TRUE;
            bb.hRgnBlur = blur_rgn_;
        } else {
            bb.dwFlags = DWM_BB_ENABLE;
            bb.fEnable = FALSE;
            bb.hRgnBlur = nullptr;
        }

        // Apply Blur Behind
        HRESULT hr = ::DwmEnableBlurBehindWindow(hWnd_, &bb);
        return SUCCEEDED(hr);
    }

    bool WindowImplWin::setBlurBehindOnWin7(bool enabled) {
        // Create and populate the Blur Behind structure
        DWM_BLURBEHIND bb = { 0 };

        // Enable Blur Behind and apply to the entire client area
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = enabled ? TRUE : FALSE;
        bb.hRgnBlur = nullptr;

        // Apply Blur Behind
        HRESULT hr = ::DwmEnableBlurBehindWindow(hWnd_, &bb);
        return SUCCEEDED(hr);
    }

    bool WindowImplWin::setBlurBehindOnWin10(bool enabled) {
        /*
         * 在 Windows 10 下使用这种方法的模糊效果没有 UWP 好看，UWP 模糊半径更大。
         * 而且存在三个问题：
         * 1. 窗口最大化动画时，可以看见四周有半透明的模糊边。
         * 2. 窗口最大化后，多屏显示器下，相邻屏幕边缘会有一点模糊。
         * 3. 某些情况下会导致窗口整体渲染不正常（可能因为未激活 Windows ?）。
         * TODO: 应寻找其他解决方案，比如 Windows Terminal 的方案。
         */
        win::ACCENT_POLICY accent;
        accent.AccentState = enabled ? win::ACCENT_ENABLE_BLURBEHIND : win::ACCENT_DISABLED;
        accent.AccentFlags = 0;
        accent.AnimationId = 0;
        // AABBGGRR
        accent.GradientColor = 0;

        win::WINDOWCOMPOSITIONATTRIBDATA data;
        data.Attrib = win::WCA_ACCENT_POLICY;
        data.pvData = &accent;
        data.cbData = sizeof(accent);

        BOOL result = win::UDSetWindowCompositionAttribute(hWnd_, &data);
        return (result != 0);
    }

    bool WindowImplWin::setTranslucent(bool trans_enabled) {
        auto context = delegate_->onGetContext();
        if (context.getTranslucentType() & TRANS_BLURBEHIND) {
            return setBlurBehind(trans_enabled);
        } else if (context.getTranslucentType() & TRANS_TRANSPARENT) {
            return setTransparent(trans_enabled);
        }
        return false;
    }

    void WindowImplWin::enableCurTranslucent(TranslucentType cur) {
        auto context = delegate_->onGetContext();
        if (cur & TRANS_BLURBEHIND) {
            if (cur & TRANS_SYSTEM) {
                setBlurBehind(context.getCurrentThemeConfig().transparency_enabled);
            } else {
                setBlurBehind(true);
            }
        } else if (cur & TRANS_TRANSPARENT) {
            if (cur & TRANS_SYSTEM) {
                setTransparent(context.getCurrentThemeConfig().transparency_enabled);
            } else {
                setTransparent(true);
            }
        } else if (cur & TRANS_LAYERED) {
            setLayered(true);
        }
    }

    void WindowImplWin::disablePrevTranslucent(TranslucentType cur) {
        auto context = delegate_->onGetContext();
        if (context.getTranslucentType() & TRANS_BLURBEHIND) {
            if ((cur & TRANS_BLURBEHIND) == 0) {
                setBlurBehind(false);
            }
        } else if (context.getTranslucentType() & TRANS_TRANSPARENT) {
            if ((cur & TRANS_TRANSPARENT) == 0) {
                setTransparent(false);
            }
        } else if (context.getTranslucentType() & TRANS_LAYERED) {
            if ((cur & TRANS_LAYERED) == 0) {
                setLayered(false);
            }
        }
    }

    void WindowImplWin::enableFullscreen(DWORD cur_style) {
        ::GetWindowPlacement(hWnd_, &saved_place_);

        auto display = DisplayWin::fromWindowImpl(this);
        if (!display->isValid()) {
            return;
        }

        auto bounds = display->getPixelBounds();

        ::SetWindowLongPtr(hWnd_, GWL_STYLE, cur_style & ~WS_OVERLAPPEDWINDOW);
        ::SetWindowPos(
            hWnd_, HWND_TOP,
            bounds.x(), bounds.y(), bounds.width(), bounds.height(),
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

}
}