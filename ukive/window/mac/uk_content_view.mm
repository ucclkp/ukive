// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "uk_content_view.h"

#import <CoreGraphics/CoreGraphics.h>

#include "utils/log.h"

#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/dirty_region.h"
#include "ukive/graphics/point.hpp"
#include "ukive/window/mac/window_impl_mac.h"
#include "ukive/window/window_native_delegate.h"


@interface UKContentView() {
    ukive::mac::WindowImplMac* windowImpl;
    NSTrackingArea* tracking_area_;
}
@end

@implementation UKContentView
@synthesize windowImpl;

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    windowImpl = nullptr;
    tracking_area_ = nil;
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void)updateTrackingAreas {
    if (tracking_area_) {
        [self removeTrackingArea:tracking_area_];
        [tracking_area_ release];
    }

    tracking_area_ = [[NSTrackingArea alloc]
                      initWithRect:[self bounds]
                           options:(NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow)
                             owner:self
                          userInfo:nil];
    [self addTrackingArea:tracking_area_];

    [super updateTrackingAreas];
}

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    [self updateTrackingAreas];
}

- (void)setBoundsSize:(NSSize)newSize {
    [super setBoundsSize:newSize];
}

- (void)setNeedsLayout:(BOOL)needsLayout {
    [super setNeedsLayout:needsLayout];
}

- (void)setNeedsDisplay:(BOOL)needsDisplay {
    [super setNeedsDisplay:needsDisplay];
}

- (void)setNeedsDisplayInRect:(NSRect)invalidRect {
    [super setNeedsDisplayInRect:invalidRect];
}

- (void)mouseDown:(NSEvent *)event {
    //[super mouseDown:event];

    if (!windowImpl || !event.window) {
        return;
    }

    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];

    auto point = windowImpl->getDelegate()->onNCHitTest(loc.x, loc.y);
    if (point == ukive::HitPoint::CAPTION) {
        [event.window performWindowDragWithEvent:event];
        return;
    }

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_DOWN);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    ev.setMouseKey(ukive::InputEvent::MK_LEFT);
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)mouseUp:(NSEvent *)event {
    //[super mouseUp:event];

    if (!windowImpl || !event.window) {
        return;
    }

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_UP);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    ev.setMouseKey(ukive::InputEvent::MK_LEFT);
    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)rightMouseDown:(NSEvent *)event {
    //[super rightMouseDown:event];

    if (!windowImpl || !event.window) {
        return;
    }

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_DOWN);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    ev.setMouseKey(ukive::InputEvent::MK_RIGHT);
    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)rightMouseUp:(NSEvent *)event {
    //[super rightMouseUp:event];

    if (!windowImpl || !event.window) {
        return;
    }

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_UP);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    ev.setMouseKey(ukive::InputEvent::MK_RIGHT);
    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)otherMouseDown:(NSEvent *)event {
    //[super otherMouseDown:event];
}

- (void)otherMouseUp:(NSEvent *)event {
    //[super otherMouseUp:event];
}

- (void)mouseMoved:(NSEvent *)event {
    //[super mouseMoved:event];

    if (!windowImpl || !event.window) {
        return;
    }

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_MOVE);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)mouseDragged:(NSEvent *)event {
    //[super mouseDragged:event];

    if (!windowImpl || !event.window) {
        return;
    }

    // 按下鼠标左键拖动时，将不会产生 mouseMoved 消息，而是
    // 本消息。框架将两种消息都按照 EVM_MOVE 对待。
    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_MOVE);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)mouseEntered:(NSEvent *)event {
    //[super mouseEntered:event];
}

- (void)mouseExited:(NSEvent *)event {
    //[super mouseExited:event];

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_LEAVE_WIN);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (void)scrollWheel:(NSEvent *)event {
    //[super scrollWheel:event];

    if (!windowImpl || !event.window) {
        return;
    }

    float dy;
    ukive::InputEvent::WheelGranularity granu;
    if (event.hasPreciseScrollingDeltas == true) {
        dy = event.scrollingDeltaY * 64;
        granu = ukive::InputEvent::WG_DELTA;
    } else {
        dy = event.scrollingDeltaY * 32;
        granu = ukive::InputEvent::WG_HIGH_PRECISE;
    }

    ukive::InputEvent ev;
    ev.setEvent(ukive::InputEvent::EVM_WHEEL);
    ev.setPointerType(ukive::InputEvent::PT_MOUSE);
    ev.setWheelValue(dy, granu);
    auto loc = [event locationInWindow];
    loc = [self convertPoint:loc fromView:nil];
    ev.setX(loc.x);
    ev.setY(loc.y);
    ev.setRawX(loc.x);
    ev.setRawY(loc.y);
    windowImpl->getDelegate()->onInputEvent(&ev);
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event {
    /**
     * 如果这里返回 NO（默认），则在非激活窗口上按鼠标键仅会激活窗口，不会产生其他鼠标事件。
     * https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/HandlingMouseEvents/HandlingMouseEvents.html
     */
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)performKeyEquivalent:(NSEvent *)event {
    /**
     * 方向键以及一些组合键，比如 control+a 或 command+a 等会先发到这里。
     * 如果这里返回了 YES，则认为该动作已经执行完毕。如果这里返回了 NO，
     * 则：
     * 1. 如果是 macOS 10.5 及以上系统，则将触发该键的 keyDown 回调；
     * 2. 否则无其他效果。
     * 注：这种情况下菜单的响应时机需要看下。
     */
    NSString* chars = event.charactersIgnoringModifiers;
    if (chars.length == 1 && [chars characterAtIndex:0] == 'w' &&
        (event.modifierFlags & NSEventModifierFlagCommand))
    {
        auto methods = windowImpl->getCloseMethods();
        if (methods & ukive::WINDOW_CLOSE_BY_MENU) {
            windowImpl->close();
        }
        return YES;
    }

    return NO;
}

- (void)keyDown:(NSEvent *)event {
    //[super keyDown:event];

    {
        int key;
        if (ukive::Keyboard::mapKey(event.keyCode, &key)) {
            ukive::InputEvent ev;
            ev.setEvent(ukive::InputEvent::EVK_DOWN);
            ev.setPointerType(ukive::InputEvent::PT_KEYBOARD);
            ev.setKeyboardKey(key, event.ARepeat == YES);
            windowImpl->getDelegate()->onInputEvent(&ev);
        }
    }

    if ((event.modifierFlags & NSEventModifierFlagControl) ||
        (event.modifierFlags & NSEventModifierFlagCommand) ||
        (event.modifierFlags & NSEventModifierFlagOption))
    {
        return;
    }

    NSString* str = [event characters];
    if (str.length > 0) {
        std::u16string u16_str;
        u16_str.resize(str.length);
        [str getCharacters:(unichar*)u16_str.data()];

        if (!ukive::Keyboard::filterChars(u16_str)) {
            ukive::InputEvent ev;
            ev.setEvent(ukive::InputEvent::EVK_CHARS);
            ev.setPointerType(ukive::InputEvent::PT_KEYBOARD);
            ev.setKeyboardChars(u16_str, event.ARepeat == YES);
            windowImpl->getDelegate()->onInputEvent(&ev);
        }
    }
}

- (void)keyUp:(NSEvent *)event {
    //[super keyUp:event];

    int key;
    if (ukive::Keyboard::mapKey(event.keyCode, &key)) {
        ukive::InputEvent ev;
        ev.setEvent(ukive::InputEvent::EVK_UP);
        ev.setPointerType(ukive::InputEvent::PT_KEYBOARD);
        ev.setKeyboardKey(key, event.ARepeat == YES);
        windowImpl->getDelegate()->onInputEvent(&ev);

        if (key == ukive::Keyboard::KEY_ESC &&
            (windowImpl->getCloseMethods() & ukive::WINDOW_CLOSE_BY_ESC))
        {
            windowImpl->close();
        }
    }
}

- (void)flagsChanged:(NSEvent *)event {
    [super flagsChanged:event];
}

- (void)drawRect:(NSRect)rect {
    // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CocoaViewsGuide/Optimizing/Optimizing.html#//apple_ref/doc/uid/TP40002978-CH11-112409
    if (windowImpl) {
        ukive::Rect u_rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        ukive::DirtyRegion region;
        region.setOne(u_rect);
        windowImpl->getDelegate()->onDraw(region);
        LOG(Log::INFO) << "drawRect: [" << u_rect.x() << ", " << u_rect.y() << ", " << u_rect.width() << ", " << u_rect.height() << "]";
    }
}

- (BOOL)isFlipped {
    return YES;
}

- (BOOL)isOpaque {
    return YES;
}

- (BOOL)wantsDefaultClipping {
    return NO;
}

@end
