// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/mac/window_impl_mac.h"

#include <cmath>

#include "utils/log.h"

#include "ukive/system/ui_utils.h"
#include "ukive/graphics/dirty_region.h"
#include "ukive/graphics/mac/display_mac.h"
#include "ukive/window/context.h"
#include "ukive/window/window_listener.h"
#include "ukive/window/window_native_delegate.h"

#import "uk_ns_window.h"
#import "uk_root_view.h"
#import "uk_content_view.h"


namespace ukive {
namespace mac {

    const int kDefaultX = 0;
    const int kDefaultY = 0;
    const int kDefaultWidth = 400;
    const int kDefaultHeight = 400;
    const char16_t kDefaultTitle[] = u"Ukive Window";

    WindowImplMac::WindowImplMac(WindowNativeDelegate* d)
        : delegate_(d),
          native_view_(nil),
          native_(nil),
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
          close_method_(WINDOW_CLOSE_BY_BUTTON | WINDOW_CLOSE_BY_MENU)
    {
        ubassert(delegate_);
    }

    WindowImplMac::~WindowImplMac() {}

    bool WindowImplMac::initialization() {
        NSWindowStyleMask mask = 0;
        if (is_resizable_) {
            mask |= NSWindowStyleMaskResizable;
        }
        if (is_minimizable_) {
            mask |= NSWindowStyleMaskMiniaturizable;
        }
        if (is_closable_) {
            mask |= NSWindowStyleMaskClosable;
        }

        if (init_params_.frame_type == WINDOW_FRAME_CUSTOM) {
            //mask |= NSWindowStyleMaskFullSizeContentView;
        } else {
            mask |= NSWindowStyleMaskTitled;
        }

        native_ = [[UKNSWindow alloc] initWithContentRect:NSMakeRect(x_, y_, width_, height_)
                                      styleMask:mask
                                      backing:NSBackingStoreBuffered defer:NO];

        std::basic_string<unichar> u_title(title_.begin(), title_.end());
        NSString* str = [[NSString alloc] initWithCharacters
                         :u_title.c_str()
                         length:u_title.size()];
        [native_ setTitle:str];
        [native_ setDelegate:native_];
        [native_ setReleasedWhenClosed:YES];
        [native_ setImpl:this];
        [native_ setOpaque:YES];
        //[native_ setTitleVisibility:NSWindowTitleHidden];
        //[native_ setTitlebarAppearsTransparent:YES];
        [str release];

        if (is_keep_on_top_) {
            [native_ setLevel:NSFloatingWindowLevel];
        }
        if (is_ignore_mouse_events_) {
            [native_ setIgnoresMouseEvents:YES];
        }

        delegate_->onCreate();

        auto content_view = [[UKContentView alloc] init];
        [content_view setWindowImpl:this];
        [content_view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

        auto root_view = [[UKRootView alloc] init];
        [root_view addSubview:content_view];

        //[root_view setWantsLayer:YES];
        [native_ setContentView:root_view];
        // 这个好像没用。只能用 Tracking Area 了。
        //[native_ setAcceptsMouseMovedEvents:YES];

        [root_view release];
        [content_view release];
        native_view_ = content_view;

        auto context = delegate_->onGetContext();
        context.setDefaultDpi(72);
        context.setScale(1);
        context.setAutoScale([native_ backingScaleFactor]);

        is_created_ = true;
        delegate_->onCreated();
        return true;
    }

    void WindowImplMac::onActivate(bool become_key) {
        delegate_->onActivate(become_key);
    }

    void WindowImplMac::onMove() {
        prev_x_ = x_;
        prev_y_ = y_;
        x_ = native_.frame.origin.x;
        y_ = native_.frame.origin.y;

        delegate_->onMove(x_, y_);
    }

    void WindowImplMac::onResize() {
        prev_width_ = width_;
        prev_height_ = height_;
        width_ = native_.frame.size.width;
        height_ = native_.frame.size.height;

        int type;
        if (isMinimized()) {
            type = WINDOW_RESIZE_MINIMIZED;
        } else if (isMaximized()) {
            type = WINDOW_RESIZE_MAXIMIZED;
        } else {
            type = WINDOW_RESIZE_RESTORED;
        }

        delegate_->onResize(type, width_, height_);
    }

    void WindowImplMac::onMoving() {
        delegate_->onMoving();
    }

    void WindowImplMac::onResizing(Size* new_size) {
        Size size(*new_size);
        if (delegate_->onResizing(&size)) {
            new_size->set(size.width, size.height);
        }
    }

    bool WindowImplMac::onClose() {
        return delegate_->onClose();
    }

    void WindowImplMac::onDestroy() {
        [native_ release];
        is_created_ = false;
        is_showing_ = false;
        delegate_->onDestroy();
    }

    UKNSWindow* WindowImplMac::getNSWindow() const {
        return native_;
    }

   WindowNativeDelegate* WindowImplMac::getDelegate() const {
       return delegate_;
   }

    bool WindowImplMac::init(const InitParams& params) {
        if (is_created_) {
            return true;
        }

        init_params_ = params;
        return initialization();
    }

    void WindowImplMac::show() {
        if (!is_created_) {
            return;
        }

        [native_ makeKeyAndOrderFront:nil];
        is_showing_ = true;
    }

    void WindowImplMac::hide() {
        if (!is_created_) {
            return;
        }

        [native_ orderOut:nil];
        is_showing_ = false;
    }

    void WindowImplMac::minimize() {
        if (!is_created_) {
            return;
        }

        if (!isMinimizable()) {
            return;
        }
        if (isMinimized()) {
            return;
        }

        [native_ miniaturize:nil];
    }

    void WindowImplMac::maximize() {
        if (!is_created_) {
            return;
        }

        if (!isMaximizable() || !isResizable()) {
            return;
        }

        if (isMaximized()) {
            return;
        }

        if (init_params_.frame_type == WINDOW_FRAME_CUSTOM) {
            // 当前没找到更好的办法，先这样吧
            native_.styleMask |= NSWindowStyleMaskTitled;
        }
        [native_ toggleFullScreen:nil];
    }

    void WindowImplMac::restore() {
        if (!is_created_) {
            return;
        }

        if (isMinimized()) {
            [native_ deminiaturize:nil];
        } else if (isMaximized()) {
            [native_ toggleFullScreen:nil];
        }
    }

    void WindowImplMac::focus() {
        if (is_created_) {
            [native_ orderFront:nil];
        }
    }

    void WindowImplMac::close() {
        if (!is_created_) {
            return;
        }

        if (!isClosable()) {
            return;
        }

        if (delegate_->onClose()) {
            [native_ close];
        }
    }

    void WindowImplMac::center() {
        auto display_bounds = DisplayMac::fromWindowImpl(this)->getBounds();
        x_ = display_bounds.left + std::round((display_bounds.width() - width_) / 2.f);
        y_ = display_bounds.top + std::round((display_bounds.height() - height_) / 2.f);

        if (native_) {
            [native_ setFrameTopLeftPoint:NSMakePoint(x_, display_bounds.height() - y_)];
        }
    }

    void WindowImplMac::invalidate(const DirtyRegion& region) {
        delegate_->onPostRender();
    }

    void WindowImplMac::requestLayout() {
        delegate_->onPostLayout();
    }

    void WindowImplMac::doDraw(const DirtyRegion& region) {
        if (native_view_) {
            if (!region.rect0.empty()) {
                auto& rect = region.rect0;
                [native_view_ setNeedsDisplayInRect:NSMakeRect(rect.left, rect.top, rect.width(), rect.height())];
                LOG(Log::INFO) << "displayRect1: [" << rect.left << ", " << rect.top << ", " << rect.width() << ", " << rect.height() << "]";
            }
            if (!region.rect1.empty()) {
                auto& rect = region.rect1;
                [native_view_ displayRect:NSMakeRect(rect.left, rect.top, rect.width(), rect.height())];
                LOG(Log::INFO) << "displayRect2: [" << rect.left << ", " << rect.top << ", " << rect.width() << ", " << rect.height() << "]";
            }
        }
    }

    void WindowImplMac::doLayout() {
        delegate_->onLayout();
    }

    void WindowImplMac::setTitle(const std::u16string_view& title) {
        title_ = title;
        if (native_) {
            std::basic_string<unichar> u_title(title_.begin(), title_.end());
            NSString* str = [[NSString alloc] initWithCharacters
                             :u_title.c_str() length:u_title.size()];
            [native_ setTitle:str];
            [str release];
        }
    }

    void WindowImplMac::setBounds(int x, int y, int width, int height) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        if (native_) {
            [native_ setFrame:NSMakeRect(x_, y_, width_, height_) display:YES];
        }
    }

    void WindowImplMac::setCurrentCursor(Cursor cursor) {
        cursor_ = cursor;
        // https://developer.apple.com/documentation/appkit/nscursor?language=objc
        switch (cursor) {
            case Cursor::ARROW:  [[NSCursor arrowCursor] set]; break;
            case Cursor::IBEAM:  [[NSCursor IBeamCursor] set]; break;
            case Cursor::HAND:   [[NSCursor pointingHandCursor] set]; break;
            case Cursor::CROSS:  [[NSCursor crosshairCursor] set]; break;
            case Cursor::SIZENS: [[NSCursor resizeUpDownCursor] set]; break;
            case Cursor::SIZEWE: [[NSCursor resizeLeftRightCursor] set]; break;
            case Cursor::_NO:    [[NSCursor operationNotAllowedCursor] set]; break;
            default: [[NSCursor arrowCursor] set]; break;
        }
    }

    void WindowImplMac::setTranslucentType(TranslucentType type) {
    }

    void WindowImplMac::setFullscreen(bool enabled) {
        if (isMaximized()) {
            restore();
        } else {
            maximize();
        }
    }

    void WindowImplMac::setResizable(bool enabled) {
        if (is_resizable_ == enabled) {
            return;
        }
        is_resizable_ = enabled;

        if (!native_) {
            return;
        }

        if (enabled) {
            native_.styleMask |= NSWindowStyleMaskResizable;
        } else {
            native_.styleMask &= ~ NSWindowStyleMaskResizable;
        }
    }

    void WindowImplMac::setMinimizable(bool enabled) {
        if (is_minimizable_ == enabled) {
            return;
        }
        is_minimizable_ = enabled;

        if (!native_) {
            return;
        }

        delegate_->onWindowButtonChanged(WindowButton::Min);

        if (enabled) {
            native_.styleMask |= NSWindowStyleMaskMiniaturizable;
        } else {
            native_.styleMask &= ~ NSWindowStyleMaskMiniaturizable;
        }
    }

    void WindowImplMac::setMaximizable(bool enabled) {
        if (is_maximizable_ == enabled) {
            return;
        }
        is_maximizable_ = enabled;

        delegate_->onWindowButtonChanged(WindowButton::Max);

        // macOS 上没有最大化的概念，只有全屏
    }

    void WindowImplMac::setClosable(bool enabled) {
        if (is_closable_ == enabled) {
            return;
        }
        is_closable_ = enabled;

        if (!native_) {
            return;
        }

        delegate_->onWindowButtonChanged(WindowButton::Close);

        if (enabled) {
            native_.styleMask |= NSWindowStyleMaskClosable;
        } else {
            native_.styleMask &= ~ NSWindowStyleMaskClosable;
        }
    }

    void WindowImplMac::setCloseMethods(uint32_t methods) {
        if (close_method_ == methods) {
            return;
        }
        close_method_ = methods;
    }

    void WindowImplMac::setKeepOnTop(bool enabled) {
        if (is_keep_on_top_ == enabled) {
            return;
        }
        is_keep_on_top_ = enabled;

        if (!native_) {
            return;
        }

        if (enabled) {
            [native_ setLevel:NSFloatingWindowLevel];
        } else {
            [native_ setLevel:NSNormalWindowLevel];
        }
    }

    void WindowImplMac::setShowInTaskBar(bool enabled) {
        if (is_show_in_taskbar_ == enabled) {
            return;
        }
        is_show_in_taskbar_ = enabled;

        // macOS 的任务栏机制与 Windows 不同，该方法似乎没什么需要做的
    }

    void WindowImplMac::setIgnoreMouseEvents(bool ignore) {
        if (is_ignore_mouse_events_ == ignore) {
            return;
        }
        is_ignore_mouse_events_ = ignore;

        if (!native_) {
            return;
        }

        [native_ setIgnoresMouseEvents:ignore ? YES : NO];
    }

    std::u16string WindowImplMac::getTitle() const {
        return title_;
    }

    Rect WindowImplMac::getBounds() const {
        return Rect(x_, y_, width_, height_);
    }

    Rect WindowImplMac::getContentBounds() const {
        if (!native_view_) {
            return {};
        }

        auto size = [native_view_ bounds].size;
        return Rect(0, 0, size.width, size.height);
    }

    Cursor WindowImplMac::getCurrentCursor() const {
        return cursor_;
    }

    WindowFrameType WindowImplMac::getFrameType() const {
        return init_params_.frame_type;
    }

    uint32_t WindowImplMac::getCloseMethods() const {
        return close_method_;
    }

    bool WindowImplMac::isCreated() const {
        return is_created_;
    }

    bool WindowImplMac::isShowing() const {
        return is_showing_;
    }

    bool WindowImplMac::isFullscreen() const {
        return isMaximized();
    }

    bool WindowImplMac::isResizable() const {
        return is_resizable_;
    }

    bool WindowImplMac::isMinimizable() const {
        return is_minimizable_;
    }

    bool WindowImplMac::isMaximizable() const {
        return is_maximizable_;
    }

    bool WindowImplMac::isClosable() const {
        return is_closable_;
    }

    bool WindowImplMac::isKeepOnTop() const {
        return is_keep_on_top_;
    }

    bool WindowImplMac::isShowInTaskBar() const {
        return is_show_in_taskbar_;
    }

    bool WindowImplMac::isIgnoreMouseEvents() const {
        return is_ignore_mouse_events_;
    }

    bool WindowImplMac::hasSizeBorder() const {
        return true;
    }

    bool WindowImplMac::isMinimized() const {
        return [native_ isMiniaturized] == YES;
    }

    bool WindowImplMac::isMaximized() const {
        return native_.styleMask & NSWindowStyleMaskFullScreen;
    }

    void WindowImplMac::setMouseCapture() {

    }

    void WindowImplMac::releaseMouseCapture() {

    }

    void WindowImplMac::convScreenToClient(Point* p) const {
        if (native_ && native_view_) {
            auto ns_p = [native_ convertPointFromScreen:NSMakePoint(p->x(), p->y())];
            ns_p = [native_view_ convertPoint:ns_p fromView:nil];
            p->x() = ns_p.x;
            p->y() = ns_p.y;
        }
    }

    void WindowImplMac::convClientToScreen(Point* p) const {
        if (native_ && native_view_) {
            auto ns_p = [native_view_ convertPoint:NSMakePoint(p->x(), p->y()) toView:nil];
            ns_p = [native_ convertPointToScreen:ns_p];
            p->x() = ns_p.x;
            p->y() = ns_p.y;
        }
    }

    float WindowImplMac::scaleToNative(float val) const {
        return val;
    }

    float WindowImplMac::scaleFromNative(float val) const {
        return val;
    }

}
}
