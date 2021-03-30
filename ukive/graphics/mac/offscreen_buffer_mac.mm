// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/offscreen_buffer_mac.h"

#import <Cocoa/Cocoa.h>

#include "ukive/graphics/mac/images/image_frame_mac.h"
#include "ukive/graphics/mac/images/image_options_mac_utils.h"
#include "ukive/window/window_dpi_utils.h"


namespace ukive {

    OffscreenBufferMac::OffscreenBufferMac()
        : width_(0), height_(0) {
    }

    bool OffscreenBufferMac::createBuffer() {
        auto color_space = CGColorSpaceCreateDeviceRGB();

        float sx, sy;
        switch (img_options_.dpi_type) {
            case ImageDPIType::SPECIFIED:
                sx = img_options_.dpi_x / float(kDefaultDpi);
                sy = img_options_.dpi_y / float(kDefaultDpi);
                break;
            default:
                sx = sy = 1;
                break;
        }

        int px = int(std::ceil(width_ * sx));
        int py = int(std::ceil(height_ * sy));
        auto info = mapCGBitmapContextInfo(img_options_);

        cg_context_ = CGBitmapContextCreate(
                                            nullptr,
                                            px, py,
                                            8, px * 4,
                                            color_space,
                                            info);
        CGColorSpaceRelease(color_space);
        if (!cg_context_) {
            return false;
        }

        CGContextScaleCTM(cg_context_, sx, sy);
        return true;
    }

    bool OffscreenBufferMac::onCreate(
        size_t width, size_t height, const ImageOptions& options)
    {
        width_ = width;
        height_ = height;
        img_options_ = options;
        return createBuffer();
    }

    GRet OffscreenBufferMac::onResize(size_t width, size_t height) {
        width_ = width;
        height_ = height;
        CGContextRelease(cg_context_);
        return createBuffer() ? GRet::Succeeded : GRet::Failed;
    }

    void OffscreenBufferMac::onDPIChange(float dpi_x, float dpi_y) {
    }

    void OffscreenBufferMac::onDestroy() {
        CGContextRelease(cg_context_);
    }

    void OffscreenBufferMac::onBeginDraw() {
        prev_context_ = [NSGraphicsContext currentContext];
        auto context = [NSGraphicsContext graphicsContextWithCGContext:cg_context_ flipped:YES];
        if (context) {
            [NSGraphicsContext setCurrentContext:context];
        }
    }

    GRet OffscreenBufferMac::onEndDraw() {
        [NSGraphicsContext setCurrentContext:prev_context_];
        return GRet::Succeeded;
    }

    Size OffscreenBufferMac::getSize() const {
        return Size(int(width_), int(height_));
    }

    Size OffscreenBufferMac::getPixelSize() const {
        auto px_width = CGBitmapContextGetWidth(cg_context_);
        auto px_height = CGBitmapContextGetHeight(cg_context_);
        return Size(int(px_width), int(px_height));
    }

    CyroRenderTarget* OffscreenBufferMac::getRT() const {
        return nullptr;
    }

    const ImageOptions& OffscreenBufferMac::getImageOptions() const {
        return img_options_;
    }

    ImageFrame* OffscreenBufferMac::onExtractImage(const ImageOptions& options) {
        if (!cg_context_) {
            return nullptr;
        }
        auto img = CGBitmapContextCreateImage(cg_context_);
        if (!img) {
            return nullptr;
        }
        auto ns_img_rep = [[NSBitmapImageRep alloc] initWithCGImage:img];
        CFRelease(img);
        if (!ns_img_rep) {
            return nullptr;
        }

        auto px_width = CGBitmapContextGetWidth(cg_context_);
        auto px_height = CGBitmapContextGetHeight(cg_context_);

        switch (options.dpi_type) {
            case ImageDPIType::SPECIFIED:
            {
                float sx = img_options_.dpi_x / float(kDefaultDpi);
                float sy = img_options_.dpi_y / float(kDefaultDpi);
                [ns_img_rep setSize:NSMakeSize(px_width / sx, px_height / sy)];
                break;
            }

            default:
                [ns_img_rep setSize:NSMakeSize(px_width, px_height)];
                break;
        }

        auto img_fr = new ImageFrameMac(ns_img_rep, true, nullptr);
        return img_fr;
    }

}
