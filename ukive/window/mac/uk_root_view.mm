// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "uk_root_view.h"

#import <CoreGraphics/CoreGraphics.h>

#include "utils/log.h"

#include "ukive/event/input_event.h"
#include "ukive/event/keyboard.h"
#include "ukive/graphics/dirty_region.h"
#include "ukive/graphics/point.hpp"
#include "ukive/window/mac/window_impl_mac.h"
#include "ukive/window/window_native_delegate.h"


@interface UKRootView() {
}
@end

@implementation UKRootView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (BOOL)isOpaque {
    return YES;
}

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
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

@end
