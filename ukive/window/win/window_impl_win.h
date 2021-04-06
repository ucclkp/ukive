// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WIN_WINDOW_IMPL_H_
#define UKIVE_WINDOW_WIN_WINDOW_IMPL_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include <map>
#include <memory>
#include <vector>

#include "ukive/graphics/cursor.h"
#include "ukive/window/window_native.h"

#define WM_NCDRAWCLASSIC1  0xAE
#define WM_NCDRAWCLASSIC2  0xAF
#define WM_NCMOUSEFIRST    WM_NCMOUSEMOVE
#define WM_NCMOUSELAST     WM_NCXBUTTONDBLCLK

#define WINDOW_MSG_HANDLER(msg, func)  \
    if (uMsg == msg) {                 \
        auto result = func(wParam, lParam, handled);  \
        if (*handled) return result; }

#define WINDOW_MSG_RANGE_HANDLER(first_msg, last_msg, func)  \
    if (uMsg >= first_msg && uMsg <= last_msg) {             \
        auto result = func(uMsg, wParam, lParam, handled);   \
        if (*handled) return result; }


namespace ukive {

    class Color;
    class InputEvent;
    class NonClientFrame;
    class WindowNativeDelegate;

    class WindowImplWin : public WindowNative {
    public:
        explicit WindowImplWin(WindowNativeDelegate* d);
        ~WindowImplWin();

        bool init(const InitParams& params) override;
        void show() override;
        void hide() override;
        void minimize() override;
        void maximize() override;
        void restore() override;
        void focus() override;
        void close() override;
        void center() override;

        void invalidate(const DirtyRegion& region) override;
        void requestLayout() override;

        void doLayout() override;
        void doDraw(const DirtyRegion& region) override;

        void setTitle(const std::u16string& title) override;
        void setBounds(int x, int y, int width, int height) override;
        void setCurrentCursor(Cursor cursor) override;
        void setTranslucentType(TranslucentType type) override;
        void setFullscreen(bool enabled) override;
        void setResizable(bool enabled) override;
        void setMaximizable(bool enabled) override;
        void setMinimizable(bool enabled) override;
        void setClosable(bool enabled) override;
        void setCloseMethods(uint32_t methods) override;
        void setKeepOnTop(bool enabled) override;
        void setShowInTaskBar(bool enabled) override;
        void setIgnoreMouseEvents(bool ignore) override;

        std::u16string getTitle() const override;
        Rect getBounds() const override;
        Rect getContentBounds() const override;
        Cursor getCurrentCursor() const override;
        WindowFrameType getFrameType() const override;
        uint32_t getCloseMethods() const override;

        bool isCreated() const override;
        bool isShowing() const override;
        bool isMinimized() const override;
        bool isMaximized() const override;
        bool isFullscreen() const override;
        bool isResizable() const override;
        bool isMaximizable() const override;
        bool isMinimizable() const override;
        bool isClosable() const override;
        bool isKeepOnTop() const override;
        bool isShowInTaskBar() const override;
        bool isIgnoreMouseEvents() const override;
        bool hasSizeBorder() const override;

        void setMouseCapture() override;
        void releaseMouseCapture() override;

        void convScreenToClient(Point* p) const override;
        void convClientToScreen(Point* p) const override;

        float scaleToNative(float val) const override;
        float scaleFromNative(float val) const override;

        bool initialization(HWND parent = nullptr);
        void setWindowStyle(int style, bool ex, bool enabled);
        void sendFrameChanged();
        void forceResize();

        void setMouseTrack();
        bool isMouseTrackEnabled() const;

        bool showTitlebarMenu();

        bool isPopup() const;
        bool isLayered() const;
        bool isTransparent() const;
        bool hasCaptain() const;
        bool hasThickFrame() const;
        HWND getHandle() const;
        int getClientOffsetX() const;
        int getClientOffsetY() const;
        int getClientWidth() const;
        int getClientHeight() const;
        int getContentWidth() const;
        int getContentHeight() const;
        int getTextureWidth() const;
        int getTextureHeight() const;
        float getUserScale() const;
        void getThemeConfig(ThemeConfig* config) const;

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        struct TouchInputCache {
            std::unique_ptr<TOUCHINPUT[]> cache;
            UINT size = 0;
        };

        void setWindowRectShape(bool enabled);
        void createFrameIfNecessary();
        void updateAppBarAwareRect();
        static void setTabletPenServiceProperties(HWND hWnd);
        static void disableTouchFeedback(HWND hWnd);
        int getPointerTypeFromMouseMsg();

        LRESULT processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP);
        LRESULT processWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled);

        LRESULT onNCCreate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onCreate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCDrawClassic1(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCDrawClassic2(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCPaint(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onPaint(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCActivate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCHitTest(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCCalCSize(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMouseRange(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMouseHover(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMouseLeave(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onClose(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDestroy(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCDestroy(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onShowWindow(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onActivate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDPIChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onStyleChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onCopyData(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onEraseBkgnd(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetCursor(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetFocus(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onKillFocus(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetText(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetIcon(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMove(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMoving(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSizing(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onKeyDown(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onKeyUp(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSysKeyDown(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSysKeyUp(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSysCommand(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onCommand(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onInitMenu(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onInitMenuPopup(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onChar(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onUniChar(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onTouch(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDisplayChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDwmCompositionChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDwmColorizationColorChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSettingChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onThemeChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onWindowPosChanging(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onWindowPosChanged(WPARAM wParam, LPARAM lParam, bool* handled);

        void onActivate(int param);
        void onMove(int x, int y);
        void onResize(int param, int width, int height);
        bool onMoving(Rect* rect);
        bool onResizing(WPARAM edge, Rect* rect);
        bool onTouch(const TOUCHINPUT* inputs, int size);
        bool onInputEvent(InputEvent* e);
        void onDpiChanged(int dpi_x, int dpi_y);
        void onStyleChanged(bool normal, bool ext, const STYLESTRUCT* ss);
        bool onDataCopy(ULONG_PTR id, DWORD size, void* data);

        void setLayered(bool enabled);
        void setBlurBehind(bool enabled);
        void setTransparent(bool enabled);
        void setBlurBehindOnWin7(bool enabled);
        void setBlurBehindOnWin10(bool enabled);
        void setTranslucent(bool trans_enabled);

        void enableCurTranslucent(TranslucentType cur);
        void disablePrevTranslucent(TranslucentType cur);

        void enableFullscreen(DWORD cur_style);

        WindowNativeDelegate* delegate_;
        std::unique_ptr<NonClientFrame> non_client_frame_;

        HWND hWnd_;
        HRGN blur_rgn_;
        Cursor cursor_;
        InitParams init_params_;

        int x_, y_;
        int prev_x_, prev_y_;
        int width_, height_;
        int prev_width_, prev_height_;
        std::wstring title_;
        WINDOWPLACEMENT saved_place_;
        bool is_first_show_maximized_ = true;

        bool is_created_;
        bool is_showing_;
        bool is_enable_mouse_track_;
        bool is_first_nccalc_;
        bool is_fullscreen_ = false;
        bool is_resizable_ = true;
        bool is_maximizable_ = true;
        bool is_minimizable_ = true;
        bool is_closable_ = true;
        uint32_t close_methods_;
        bool is_keep_on_top_ = false;
        bool is_show_in_task_bar_ = true;
        bool is_ignore_mouse_events_ = false;

        TouchInputCache ti_cache_;
        std::map<DWORD, TOUCHINPUT> prev_ti_;

        int prev_touch_x_ = 0;
        int prev_touch_y_ = 0;
        bool is_prev_touched_ = false;
        uint64_t prev_touch_time_ = 0;
    };

}

#endif  // UKIVE_WINDOW_WIN_WINDOW_IMPL_H_