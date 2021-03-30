// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

namespace ukive {
    class WindowImplMac;
}


@interface UKNSWindow : NSWindow<NSWindowDelegate>

@property(assign) ukive::WindowImplMac* impl;

- (instancetype)initWithContentRect:(NSRect)contentRect
                         styleMask:(NSWindowStyleMask)style
                           backing:(NSBackingStoreType)backingStoreType
                             defer:(BOOL)flag;
- (void)dealloc;

- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;

- (void)windowDidBecomeKey:(NSNotification *)notification;
- (void)windowDidResignKey:(NSNotification *)notification;
- (void)windowWillMove:(NSNotification *)notification;
- (void)windowDidMove:(NSNotification *)notification;
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize;
- (void)windowDidResize:(NSNotification *)notification;

- (void)windowDidMiniaturize:(NSNotification *)notification;
- (void)windowDidDeminiaturize:(NSNotification *)notification;

- (void)windowWillExitFullScreen:(NSNotification *)notification;
- (BOOL)windowShouldClose:(NSWindow *)sender;
- (void)windowWillClose:(NSNotification *)notification;

@end
