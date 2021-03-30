// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>


namespace ukive {
    class Canvas;
    class TextInlineObject;
}

@interface TextInlineCell : NSTextAttachmentCell

@property(assign) ukive::Canvas* _Nonnull canvas;
@property(assign) ukive::TextInlineObject* _Nonnull delegate;

- (void)drawWithFrame:(NSRect)cellFrame inView:(nullable NSView *)controlView;
- (BOOL)wantsToTrackMouse;
- (void)highlight:(BOOL)flag withFrame:(NSRect)cellFrame inView:(nullable NSView *)controlView;
- (BOOL)trackMouse:(NSEvent *_Nonnull)theEvent inRect:(NSRect)cellFrame ofView:(nullable NSView *)controlView untilMouseUp:(BOOL)flag;
- (NSSize)cellSize;
- (NSPoint)cellBaselineOffset;

- (void)drawWithFrame:(NSRect)cellFrame inView:(nullable NSView *)controlView characterIndex:(NSUInteger)charIndex;
- (void)drawWithFrame:(NSRect)cellFrame inView:(nullable NSView *)controlView characterIndex:(NSUInteger)charIndex layoutManager:(NSLayoutManager *_Nonnull)layoutManager;
- (BOOL)wantsToTrackMouseForEvent:(NSEvent *_Nonnull)theEvent inRect:(NSRect)cellFrame ofView:(nullable NSView *)controlView atCharacterIndex:(NSUInteger)charIndex;
- (BOOL)trackMouse:(NSEvent *_Nonnull)theEvent inRect:(NSRect)cellFrame ofView:(nullable NSView *)controlView atCharacterIndex:(NSUInteger)charIndex untilMouseUp:(BOOL)flag;
- (NSRect)cellFrameForTextContainer:(NSTextContainer *_Nonnull)textContainer proposedLineFragment:(NSRect)lineFrag glyphPosition:(NSPoint)position characterIndex:(NSUInteger)charIndex;

@end
