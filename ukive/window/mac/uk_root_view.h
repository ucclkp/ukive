// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>


@interface UKRootView : NSView

- (instancetype)initWithFrame:(NSRect)frameRect;
- (void)dealloc;

- (void)setFrameSize:(NSSize)newSize;
- (void)setBoundsSize:(NSSize)newSize;
- (void)setNeedsLayout:(BOOL)needsLayout;
- (void)setNeedsDisplay:(BOOL)needsDisplay;
- (void)setNeedsDisplayInRect:(NSRect)invalidRect;

@end
