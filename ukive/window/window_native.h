// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_WINDOW_NATIVE_H_
#define UKIVE_WINDOW_WINDOW_NATIVE_H_

#include <string>

#include "ukive/graphics/cursor.h"
#include "ukive/graphics/point.hpp"
#include "ukive/graphics/rect.hpp"
#include "ukive/system/theme_info.h"
#include "ukive/window/window_types.h"


namespace ukive {

    class Color;
    class WindowNativeDelegate;
    class DirtyRegion;

    class WindowNative {
    public:
        static WindowNative* create(WindowNativeDelegate* w);

        struct InitParams {
            WindowNative* parent = nullptr;
            WindowFrameType frame_type = WINDOW_FRAME_CUSTOM;
        };

        virtual ~WindowNative() = default;

        virtual bool init(const InitParams& params) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void minimize() = 0;
        virtual void maximize() = 0;
        virtual void restore() = 0;
        virtual void focus() = 0;
        virtual void center() = 0;
        virtual void close() = 0;

        virtual void invalidate(const DirtyRegion& region) = 0;
        virtual void requestLayout() = 0;

        virtual void doDraw(const DirtyRegion& region) = 0;
        virtual void doLayout() = 0;

        virtual void setTitle(const std::u16string_view& title) = 0;
        virtual void setBounds(int x, int y, int width, int height) = 0;
        virtual void setCurrentCursor(Cursor cursor) = 0;
        virtual void setTranslucentType(TranslucentType type) = 0;
        virtual void setFullscreen(bool enabled) = 0;
        virtual void setResizable(bool enabled) = 0;
        virtual void setMinimizable(bool enabled) = 0;
        virtual void setMaximizable(bool enabled) = 0;
        virtual void setClosable(bool enabled) = 0;
        virtual void setCloseMethods(uint32_t methods) = 0;
        virtual void setKeepOnTop(bool enabled) = 0;
        virtual void setShowInTaskBar(bool enabled) = 0;
        virtual void setIgnoreMouseEvents(bool ignore) = 0;

        virtual std::u16string getTitle() const = 0;

        /**
         * 获取窗口在屏幕上的位置和大小
         */
        virtual Rect getBounds() const = 0;

        /**
         * 获取窗口中可绘区域在窗口中的位置和大小。
         */
        virtual Rect getContentBounds() const = 0;
        virtual Cursor getCurrentCursor() const = 0;
        virtual WindowFrameType getFrameType() const = 0;
        virtual uint32_t getCloseMethods() const = 0;

        virtual bool isCreated() const = 0;
        virtual bool isShowing() const = 0;
        virtual bool isMinimized() const = 0;
        virtual bool isMaximized() const = 0;
        virtual bool isFullscreen() const = 0;
        virtual bool isResizable() const = 0;
        virtual bool isMinimizable() const = 0;
        virtual bool isMaximizable() const = 0;
        virtual bool isClosable() const = 0;
        virtual bool isKeepOnTop() const = 0;
        virtual bool isShowInTaskBar() const = 0;
        virtual bool isIgnoreMouseEvents() const = 0;
        virtual bool hasSizeBorder() const = 0;

        virtual bool setMouseCapture() = 0;
        virtual bool releaseMouseCapture() = 0;

        virtual bool trackMouseHover(bool force) = 0;

        virtual void convScreenToClient(Point* p) const = 0;
        virtual void convClientToScreen(Point* p) const = 0;

        virtual float scaleToNative(float val) const = 0;
        virtual float scaleFromNative(float val) const = 0;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_NATIVE_H_
