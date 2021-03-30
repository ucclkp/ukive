// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#import "uk_ns_window.h"

#include "ukive/graphics/size.hpp"
#include "ukive/window/window_native_delegate.h"
#include "ukive/window/mac/window_impl_mac.h"


@interface UKNSWindow() {
    ukive::WindowImplMac* impl;
}
@end

@implementation UKNSWindow
@synthesize impl;

- (instancetype)initWithContentRect:(NSRect)contentRect
                         styleMask:(NSWindowStyleMask)style
                           backing:(NSBackingStoreType)backingStoreType
                             defer:(BOOL)flag
{
    self = [super initWithContentRect:contentRect
                            styleMask:style
                              backing:backingStoreType
                                defer:flag];
    impl = nullptr;
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (BOOL)canBecomeKeyWindow {
    return YES;
}

- (BOOL)canBecomeMainWindow {
    return YES;
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    if (impl) {
        impl->onActivate(true);
    }
}

- (void)windowDidResignKey:(NSNotification *)notification {
    if (impl) {
        impl->onActivate(false);
    }
}

- (void)windowWillMove:(NSNotification *)notification {
    if (impl) {
        impl->onMoving();
    }
}

- (void)windowDidMove:(NSNotification *)notification {
    if (impl) {
        impl->onMove();
    }
}

- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize {
    if (impl) {
        ukive::Size new_size(frameSize.width, frameSize.height);
        impl->onResizing(&new_size);
        return NSMakeSize(new_size.width, new_size.height);
    }
    return frameSize;
}

- (void)windowDidResize:(NSNotification *)notification {
    if (impl) {
        impl->onResize();
    }
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
}

- (void)windowWillExitFullScreen:(NSNotification *)notification {
    if (impl->getFrameType() == ukive::WINDOW_FRAME_CUSTOM) {
        self.styleMask &= ~NSWindowStyleMaskTitled;
    }
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    return impl->onClose();
}

- (void)windowWillClose:(NSNotification *)notification {
    [self setDelegate:nil];
    if (impl) {
        impl->onDestroy();
        // TODO: 需要找一个合适的时机调用 onDestroyed
        impl->getDelegate()->onDestroyed();
    }
    impl = nullptr;
}

@end
