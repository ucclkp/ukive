// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#import "text_inline_cell.h"

#include "ukive/text/text_inline_object.h"


@implementation TextInlineCell

@synthesize canvas;
@synthesize delegate;

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
    if (delegate && canvas) {
        delegate->onDrawInlineObject(canvas, cellFrame.origin.x, cellFrame.origin.y);
    }
    canvas = nullptr;
}

- (BOOL)wantsToTrackMouse {
    return NO;
}

- (void)highlight:(BOOL)flag
        withFrame:(NSRect)cellFrame
           inView:(NSView *)controlView
{
}

- (BOOL)trackMouse:(NSEvent *)theEvent
            inRect:(NSRect)cellFrame
            ofView:(NSView *)controlView
      untilMouseUp:(BOOL)flag
{
    return NO;
}

- (NSSize)cellSize {
    return [super cellSize];
}

- (NSPoint)cellBaselineOffset {
    return [super cellBaselineOffset];
}

- (void)drawWithFrame:(NSRect)cellFrame
               inView:(nullable NSView *)controlView
       characterIndex:(NSUInteger)charIndex
{
    [self drawWithFrame:cellFrame inView:controlView];
}

- (void)drawWithFrame:(NSRect)cellFrame
               inView:(nullable NSView *)controlView
       characterIndex:(NSUInteger)charIndex
        layoutManager:(NSLayoutManager *_Nonnull)layoutManager
{
    [self drawWithFrame:cellFrame inView:controlView characterIndex:charIndex];
}

- (BOOL)wantsToTrackMouseForEvent:(NSEvent *_Nonnull)theEvent
                           inRect:(NSRect)cellFrame
                           ofView:(nullable NSView *)controlView
                 atCharacterIndex:(NSUInteger)charIndex
{
    return [self wantsToTrackMouse];
}

- (BOOL)trackMouse:(NSEvent *_Nonnull)theEvent
            inRect:(NSRect)cellFrame
            ofView:(nullable NSView *)controlView
  atCharacterIndex:(NSUInteger)charIndex
      untilMouseUp:(BOOL)flag
{
    return [self trackMouse:theEvent inRect:cellFrame ofView:controlView untilMouseUp:flag];
}

- (NSRect)cellFrameForTextContainer:(NSTextContainer *_Nonnull)textContainer
               proposedLineFragment:(NSRect)lineFrag
                      glyphPosition:(NSPoint)position
                     characterIndex:(NSUInteger)charIndex
{
    if (delegate) {
        float width = 0, height = 0, baseline = 0;
        delegate->onGetMetrics(&width, &height, &baseline);
        return NSMakeRect(0, 0, width, height);
    }
    return NSZeroRect;
}

@end
