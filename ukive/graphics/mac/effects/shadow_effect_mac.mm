// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/effects/shadow_effect_mac.h"

#include "ukive/graphics/mac/offscreen_buffer_mac.h"
#include "ukive/graphics/mac/images/image_frame_mac.h"

#import <Cocoa/Cocoa.h>


namespace ukive {
namespace mac {

    ShadowEffectMac::ShadowEffectMac() {}

    bool ShadowEffectMac::initialize() {
        shadow_ = [[NSShadow alloc] init];
        [shadow_ setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.3f]];
        return true;
    }

    void ShadowEffectMac::destroy() {
        if (shadow_) {
            [shadow_ release];
        }
    }

    bool ShadowEffectMac::generate(Canvas* c) {
        return false;
    }

    bool ShadowEffectMac::draw(Canvas* c) {
        if (!content_ || !shadow_) {
            return false;
        }

        [NSGraphicsContext saveGraphicsState];
        [shadow_ set];

        auto img = static_cast<ImageFrameMac*>(content_.get());
        auto ns_img = img->getNative();
        auto size = ns_img.size;
        auto rect = NSMakeRect(0, 0, size.width, size.height);
        [ns_img drawInRect:rect
                  fromRect:rect
                 operation:NSCompositingOperationSourceOver
                  fraction:1.0f
            respectFlipped:!img->alreadyFilpped()
                     hints:nil];

        [NSGraphicsContext restoreGraphicsState];
        return true;
    }

    bool ShadowEffectMac::setContent(OffscreenBuffer* content) {
        auto off = static_cast<OffscreenBufferMac*>(content);
        content_ = off->onExtractImage(content->getImageOptions());
        return true;
    }

    GPtr<ImageFrame> ShadowEffectMac::getOutput() const {
        return {};
    }

    void ShadowEffectMac::resetCache() {

    }

    bool ShadowEffectMac::hasCache() const {
        return false;
    }

    bool ShadowEffectMac::setRadius(int radius) {
        if (shadow_) {
            radius_ = radius;
            [shadow_ setShadowBlurRadius:radius];
            return true;
        }
        return false;
    }

    int ShadowEffectMac::getRadius() const {
        return radius_;
    }

}
}
