// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/offscreen_buffer_mac.h"

#import <Cocoa/Cocoa.h>

#include "utils/log.h"

#include "ukive/graphics/mac/images/image_frame_mac.h"
#include "ukive/graphics/mac/images/image_options_mac_utils.h"
#include "ukive/window/window_dpi_utils.h"


namespace ukive {
namespace mac {

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
        int width, int height, const ImageOptions& options)
    {
        if (width <= 0 || height <= 0) {
            DLOG(Log::ERR) << "Invalid size value.";
            return false;
        }

        width_ = width;
        height_ = height;
        img_options_ = options;
        return createBuffer();
    }

    bool OffscreenBufferMac::onRecreate() {
        onDestroy();

        if (width_ <= 0 || height_ <= 0) {
            DLOG(Log::ERR) << "Invalid size value.";
            return false;
        }

        return createBuffer();
    }

    GRet OffscreenBufferMac::onResize(int width, int height) {
        if (width <= 0 || height <= 0) {
            DLOG(Log::WARNING) << "Invalid size value.";
            return GRet::Succeeded;
        }

        if (width == width_ && height == height_ && cg_context_) {
            return GRet::Succeeded;
        }

        width_ = width;
        height_ = height;
        CGContextRelease(cg_context_);
        return createBuffer() ? GRet::Succeeded : GRet::Failed;
    }

    void OffscreenBufferMac::onDPIChange(float dpi_x, float dpi_y) {
        if (dpi_x <= 0 || dpi_y <= 0) {
            DLOG(Log::ERR) << "Invalid dpi values.";
            return;
        }
    }

    void OffscreenBufferMac::onDestroy() {
        CGContextRelease(cg_context_);
        cg_context_ = nullptr;
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

    const NativeRT* OffscreenBufferMac::getNativeRT() const {
        return nullptr;
    }

    const ImageOptions& OffscreenBufferMac::getImageOptions() const {
        return img_options_;
    }

    GPtr<ImageFrame> OffscreenBufferMac::onExtractImage(const ImageOptions& options) {
        if (!cg_context_) {
            return {};
        }
        auto img = CGBitmapContextCreateImage(cg_context_);
        if (!img) {
            return {};
        }
        auto ns_img_rep = [[NSBitmapImageRep alloc] initWithCGImage:img];
        CFRelease(img);
        if (!ns_img_rep) {
            return {};
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

        auto img_fr = new ImageFrameMac(options, ns_img_rep, true, {});
        return GPtr<ImageFrame>(img_fr);
    }

}
}
