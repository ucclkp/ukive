// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_MAC_WINDOW_IMPL_MAC_H_
#define UKIVE_WINDOW_MAC_WINDOW_IMPL_MAC_H_

#include "utils/mac/objc_utils.hpp"

#include "ukive/graphics/size.hpp"
#include "ukive/window/window_native.h"

UTL_OBJC_CLASS(NSView);
UTL_OBJC_CLASS(UKNSWindow);


namespace ukive {

    class Color;
    class InputEvent;

namespace mac {

    class WindowImplMac : public WindowNative {
    public:
        explicit WindowImplMac(WindowNativeDelegate* d);
        ~WindowImplMac();

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

        void doDraw(const DirtyRegion& region) override;
        void doLayout() override;

        void setTitle(const std::u16string_view& title) override;
        void setBounds(int x, int y, int width, int height) override;
        void setCurrentCursor(Cursor cursor) override;
        void setTranslucentType(TranslucentType type) override;
        void setFullscreen(bool enabled) override;
        void setResizable(bool enabled) override;
        void setMinimizable(bool enabled) override;
        void setMaximizable(bool enabled) override;
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
        bool isMinimizable() const override;
        bool isMaximizable() const override;
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

        void onActivate(bool become_key);
        void onMove();
        void onResize();
        void onMoving();
        void onResizing(Size* new_size);
        bool onClose();
        void onDestroy();

        UKNSWindow* getNSWindow() const;
        WindowNativeDelegate* getDelegate() const;

    private:
        bool initialization();

        WindowNativeDelegate* delegate_;

        NSView* native_view_;
        UKNSWindow* native_;
        Cursor cursor_;
        uint32_t close_method_;
        InitParams init_params_;

        int x_, y_;
        int prev_x_, prev_y_;
        int width_, height_;
        int prev_width_, prev_height_;
        std::u16string title_;

        bool is_created_;
        bool is_showing_;
        bool is_resizable_ = true;
        bool is_minimizable_ = true;
        bool is_maximizable_ = true;
        bool is_closable_ = true;
        bool is_keep_on_top_ = false;
        bool is_show_in_taskbar_ = true;
        bool is_ignore_mouse_events_ = false;
    };

}
}

#endif  // UKIVE_WINDOW_MAC_WINDOW_IMPL_MAC_H_
