// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>


namespace ukive {
namespace mac {
    class WindowImplMac;
}
}

@interface UKContentView : NSView

@property(assign) ukive::mac::WindowImplMac* windowImpl;

- (instancetype)initWithFrame:(NSRect)frameRect;
- (void)dealloc;
- (void)updateTrackingAreas;

- (void)setFrameSize:(NSSize)newSize;
- (void)setBoundsSize:(NSSize)newSize;
- (void)setNeedsLayout:(BOOL)needsLayout;
- (void)setNeedsDisplay:(BOOL)needsDisplay;
- (void)setNeedsDisplayInRect:(NSRect)invalidRect;

- (void)mouseDown:(NSEvent *)event;
- (void)mouseUp:(NSEvent *)event;
- (void)rightMouseDown:(NSEvent *)event;
- (void)rightMouseUp:(NSEvent *)event;
- (void)otherMouseDown:(NSEvent *)event;
- (void)otherMouseUp:(NSEvent *)event;

- (void)mouseMoved:(NSEvent *)event;
- (void)mouseDragged:(NSEvent *)event;
- (void)mouseEntered:(NSEvent *)event;
- (void)mouseExited:(NSEvent *)event;
- (void)scrollWheel:(NSEvent *)event;

- (void)keyDown:(NSEvent *)event;
- (void)keyUp:(NSEvent *)event;

- (void)drawRect:(NSRect)rect;
- (BOOL)isFlipped;
- (BOOL)isOpaque;
- (BOOL)wantsDefaultClipping;

@end
