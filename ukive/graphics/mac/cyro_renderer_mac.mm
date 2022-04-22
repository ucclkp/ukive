// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/cyro_renderer_mac.h"

#include "ukive/app/application.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/paint.h"
#include "ukive/graphics/mac/path_mac.h"
#include "ukive/graphics/mac/images/lc_image_frame_mac.h"
#include "ukive/graphics/mac/images/image_frame_mac.h"
#include "ukive/text/mac/text_layout_mac.h"

#import <Cocoa/Cocoa.h>


namespace ukive {
namespace mac {

    CyroRendererMac::CyroRendererMac() {}

    CyroRendererMac::~CyroRendererMac() {
        if (is_owned_buffer_ && buffer_) {
            buffer_->onDestroy();
            delete buffer_;
        }
    }

    bool CyroRendererMac::bind(CyroBuffer* buffer, bool owned) {
        if (!buffer) {
            return false;
        }

        buffer_ = buffer;
        is_owned_buffer_ = owned;
        return true;
    }

    void CyroRendererMac::unbind() {
    }

    CyroBuffer* CyroRendererMac::getBuffer() const {
        return buffer_;
    }

    GPtr<ImageFrame> CyroRendererMac::createImage(const GPtr<LcImageFrame>& img) {
        if (!img) {
            return {};
        }
        auto cg_img = img.cast<LcImageFrameMac>()->getNative();
        if (!cg_img) {
            return {};
        }
        auto ns_img = [[NSBitmapImageRep alloc] initWithCGImage:cg_img];
        if (!ns_img) {
            return {};
        }

        float dpi_x, dpi_y;
        img->getDpi(&dpi_x, &dpi_y);

        auto image_fr = new ImageFrameMac(img->getOptions(), ns_img, false, {});
        image_fr->setDpi(dpi_x, dpi_y);

        return GPtr<ImageFrame>(image_fr);
    }

    GPtr<ImageFrame> CyroRendererMac::createImage(
        int width, int height, const ImageOptions& options)
    {
        return createImage(width, height, {}, 0, options);
    }

    GPtr<ImageFrame> CyroRendererMac::createImage(
        int width, int height,
        const GPtr<ByteData>& pixel_data, size_t stride,
        const ImageOptions& options)
    {
        NSBitmapFormat format = NSBitmapFormatAlphaFirst;
        switch (options.pixel_format) {
            case ImagePixelFormat::HDR:
                format |= NSBitmapFormatFloatingPointSamples;
                break;

            case ImagePixelFormat::RAW:
            case ImagePixelFormat::B8G8R8A8_UNORM:
            default:
                format |= NSBitmapFormatThirtyTwoBitLittleEndian;
                break;
        }

        BOOL has_alpha;
        switch (options.alpha_mode) {
            case ImageAlphaMode::IGNORED:
                has_alpha = NO;
                break;
            case ImageAlphaMode::STRAIGHT:
                has_alpha = YES;
                format |= NSBitmapFormatAlphaNonpremultiplied;
                break;
            case ImageAlphaMode::PREMULTIPLIED:
            default:
                has_alpha = YES;
                break;
        }

        unsigned char* buf;
        unsigned char** plane;
        if (pixel_data) {
            buf = static_cast<unsigned char*>(pixel_data->getData());
            plane = &buf;
        } else {
            buf = nullptr;
            plane = nullptr;
        }

        if (!plane) {
            stride = 0;
        }

        auto scale = [NSScreen mainScreen].backingScaleFactor;
        auto img = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:plane
                                                pixelsWide:width * scale
                                                pixelsHigh:height * scale
                                             bitsPerSample:8
                                           samplesPerPixel:4
                                                  hasAlpha:has_alpha
                                                  isPlanar:NO
                                            colorSpaceName:NSDeviceRGBColorSpace
                                              bitmapFormat:format
                                               bytesPerRow:stride
                                              bitsPerPixel:0];
        if (!img) {
            return {};
        }

        auto image_fr = new ImageFrameMac(options, img, false, pixel_data);

        switch (options.dpi_type) {
            case ImageDPIType::SPECIFIED:
                image_fr->setDpi(options.dpi_x, options.dpi_y);
                break;
            case ImageDPIType::DEFAULT:
            default:
                break;
        }

        return GPtr<ImageFrame>(image_fr);
    }

    void CyroRendererMac::setOpacity(float opacity) {
        if (opacity == opacity_) {
            return;
        }

        opacity_ = opacity;
    }

    float CyroRendererMac::getOpacity() const {
        return opacity_;
    }

    Matrix2x3F CyroRendererMac::getMatrix() const {
        return matrix_;
    }

    void CyroRendererMac::onBeginDraw() {
        if (buffer_) {
            buffer_->onBeginDraw();
        }
    }

    GRet CyroRendererMac::onEndDraw() {
        if (!buffer_) {
            return GRet::Failed;
        }
        return buffer_->onEndDraw();
    }

    void CyroRendererMac::clear() {
        if (buffer_) {
            auto size = buffer_->getSize();
            [[NSColor colorWithRed:0 green:0 blue:0 alpha:0] setFill];
            NSRectFill(NSMakeRect(0, 0, size.width, size.height));
        }
    }

    void CyroRendererMac::clear(const Color &c) {
        if (buffer_) {
            auto size = buffer_->getSize();
            [[NSColor colorWithRed:c.r green:c.g blue:c.b alpha:c.a] setFill];
            NSRectFill(NSMakeRect(0, 0, size.width, size.height));
        }
    }

    void CyroRendererMac::pushClip(const RectF &rect) {
        [NSGraphicsContext saveGraphicsState];
        NSRectClip(NSMakeRect(rect.left, rect.top, rect.width(), rect.height()));
    }

    void CyroRendererMac::popClip() {
        [NSGraphicsContext restoreGraphicsState];
    }

    void CyroRendererMac::save() {
        [NSGraphicsContext saveGraphicsState];
        matrix_stack_.push(matrix_);
        opacity_stack_.push(opacity_);
    }

    void CyroRendererMac::restore() {
        [NSGraphicsContext restoreGraphicsState];
        matrix_ = matrix_stack_.top();
        matrix_stack_.pop();

        opacity_ = opacity_stack_.top();
        opacity_stack_.pop();
    }

    void CyroRendererMac::scale(float sx, float sy, const PointF &c) {
        matrix_.preScale(sx, sy, c.x(), c.y());

        auto xform = [NSAffineTransform transform];
        [xform translateXBy:-c.x() yBy:-c.y()];
        [xform scaleXBy:sx yBy:sy];
        [xform translateXBy:c.x() yBy:c.y()];
        [xform concat];
    }

    void CyroRendererMac::rotate(float angle, const PointF &c) {
        matrix_.preRotate(angle, c.x(), c.y());

        auto xform = [NSAffineTransform transform];
        [xform translateXBy:-c.x() yBy:-c.y()];
        [xform rotateByDegrees:angle];
        [xform translateXBy:c.x() yBy:c.y()];
        [xform concat];
    }

    void CyroRendererMac::translate(float dx, float dy) {
        matrix_.preTranslate(dx, dy);

        auto xform = [NSAffineTransform transform];
        [xform translateXBy:dx yBy:dy];
        [xform concat];
    }

    void CyroRendererMac::concat(const Matrix2x3F &matrix) {
        matrix_ = matrix_ * matrix;

        auto xform = [NSAffineTransform transform];
        xform.transformStruct = {
            matrix.m11, matrix.m21, matrix.m12, matrix.m22,
            matrix.m13, matrix.m23
        };
        [xform concat];
    }

    void CyroRendererMac::drawPoint(const PointF &p, const Paint &paint) {
        auto color = paint.getColor();
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        [ns_color setFill];

        auto ns_rect = NSMakeRect(p.x(), p.y(), 1, 1);
        NSRectFill(ns_rect);
    }

    void CyroRendererMac::drawLine(
        const PointF &start, const PointF &end, const Paint &paint)
    {
        auto color = paint.getColor();
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        [ns_color setStroke];

        auto path = [NSBezierPath bezierPath];
        [path setLineWidth:paint.getStrokeWidth()];
        [path moveToPoint:NSMakePoint(start.x(), start.y())];
        [path lineToPoint:NSMakePoint(end.x(), end.y())];
        [path closePath];

        [path stroke];
    }

    void CyroRendererMac::drawRect(const RectF &rect, const Paint &paint) {
        auto color = paint.getColor();
        auto ns_rect = NSMakeRect(rect.left, rect.top, rect.width(), rect.height());
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        auto path = [NSBezierPath bezierPathWithRect:ns_rect];

        switch (paint.getStyle()) {
            case Paint::Style::STROKE:
                [ns_color setStroke];
                [path setLineWidth:paint.getStrokeWidth()];
                [path stroke];
                break;
            case Paint::Style::FILL:
                [ns_color setFill];
                [path fill];
                break;
            case Paint::Style::IMAGE:
            {
                [NSGraphicsContext saveGraphicsState];
                [path setClip];
                auto img = static_cast<const ImageFrameMac*>(paint.getImage());
                auto ns_img = img->getNative();
                auto size = ns_img.size;
                auto src = NSMakeRect(0, 0, size.width, size.height);
                [ns_img drawInRect:src
                       fromRect:src
                      operation:NSCompositingOperationSourceOver
                       fraction:opacity_
                 respectFlipped:!img->alreadyFilpped()
                          hints:nil];
                break;
            }
        }
    }

    void CyroRendererMac::drawRoundRect(
        const RectF &rect, float radius, const Paint &paint)
    {
        auto color = paint.getColor();
        auto ns_rect = NSMakeRect(rect.left, rect.top, rect.width(), rect.height());
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        auto path = [NSBezierPath bezierPathWithRoundedRect:ns_rect xRadius:radius yRadius:radius];

        switch (paint.getStyle()) {
            case Paint::Style::STROKE:
                [ns_color setStroke];
                [path setLineWidth:paint.getStrokeWidth()];
                [path stroke];
                break;
            case Paint::Style::FILL:
                [ns_color setFill];
                [path fill];
                break;
            case Paint::Style::IMAGE:
            {
                [NSGraphicsContext saveGraphicsState];
                [path setClip];
                auto img = static_cast<const ImageFrameMac*>(paint.getImage());
                auto ns_img = img->getNative();
                auto size = ns_img.size;
                auto src = NSMakeRect(0, 0, size.width, size.height);
                [ns_img drawInRect:src
                       fromRect:src
                      operation:NSCompositingOperationSourceOver
                       fraction:opacity_
                 respectFlipped:!img->alreadyFilpped()
                          hints:nil];
                [NSGraphicsContext restoreGraphicsState];
                break;
            }
            default:
                break;
        }
    }

    void CyroRendererMac::drawCircle(const PointF &c, float radius, const Paint &paint) {
        auto color = paint.getColor();
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        auto ns_rect = NSMakeRect(c.x() - radius, c.y() - radius, radius * 2, radius * 2);
        auto path = [NSBezierPath bezierPathWithOvalInRect:ns_rect];

        switch (paint.getStyle()) {
            case Paint::Style::STROKE:
                [ns_color setStroke];
                [path setLineWidth:paint.getStrokeWidth()];
                [path stroke];
                break;
            case Paint::Style::FILL:
                [ns_color setFill];
                [path fill];
                break;
            case Paint::Style::IMAGE:
            {
                [NSGraphicsContext saveGraphicsState];
                [path setClip];
                auto img = static_cast<const ImageFrameMac*>(paint.getImage());
                auto ns_img = img->getNative();
                auto size = ns_img.size;
                auto src = NSMakeRect(0, 0, size.width, size.height);
                [ns_img drawInRect:src
                       fromRect:src
                      operation:NSCompositingOperationSourceOver
                       fraction:opacity_
                 respectFlipped:!img->alreadyFilpped()
                          hints:nil];
                [NSGraphicsContext restoreGraphicsState];
                break;
            }
        }
    }

    void CyroRendererMac::drawEllipse(
        const PointF &c, float rx, float ry, const Paint &paint)
    {
        auto color = paint.getColor();
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        auto ns_rect = NSMakeRect(c.x() - rx, c.y() - ry, rx * 2, ry * 2);
        auto path = [NSBezierPath bezierPathWithOvalInRect:ns_rect];

        switch (paint.getStyle()) {
            case Paint::Style::STROKE:
                [ns_color setStroke];
                [path setLineWidth:paint.getStrokeWidth()];
                [path stroke];
                break;
            case Paint::Style::FILL:
                [ns_color setFill];
                [path fill];
                break;
            case Paint::Style::IMAGE:
            {
                [NSGraphicsContext saveGraphicsState];
                [path setClip];
                auto img = static_cast<const ImageFrameMac*>(paint.getImage());
                auto ns_img = img->getNative();
                auto size = ns_img.size;
                auto src = NSMakeRect(0, 0, size.width, size.height);
                [ns_img drawInRect:src
                       fromRect:src
                      operation:NSCompositingOperationSourceOver
                       fraction:opacity_
                 respectFlipped:!img->alreadyFilpped()
                          hints:nil];
                [NSGraphicsContext restoreGraphicsState];
                break;
            }
        }
    }

    void CyroRendererMac::drawPath(const Path *path, const Paint &paint) {
        auto color = paint.getColor();
        auto ns_color = [NSColor colorWithRed:color.r
                                        green:color.g
                                         blue:color.b
                                        alpha:color.a * opacity_];
        auto mac_path = static_cast<const PathMac*>(path)->getNative();

        switch (paint.getStyle()) {
            case Paint::Style::STROKE:
                [ns_color setStroke];
                [mac_path setLineWidth:paint.getStrokeWidth()];
                [mac_path stroke];
                break;
            case Paint::Style::FILL:
                [ns_color setFill];
                [mac_path fill];
                break;
            case Paint::Style::IMAGE:
            {
                [NSGraphicsContext saveGraphicsState];
                [mac_path setClip];
                auto img = static_cast<const ImageFrameMac*>(paint.getImage());
                auto ns_img = img->getNative();
                auto size = ns_img.size;
                auto dst = NSMakeRect(0, 0, size.width, size.height);
                [ns_img drawInRect:dst
                       fromRect:dst
                      operation:NSCompositingOperationSourceOver
                       fraction:opacity_
                 respectFlipped:!img->alreadyFilpped()
                          hints:nil];
                [NSGraphicsContext restoreGraphicsState];
                break;
            }
        }
    }

    void CyroRendererMac::drawImage(
        const RectF &src, const RectF &dst, float opacity, ImageFrame* img)
    {
        auto img_fr = static_cast<const ImageFrameMac*>(img);
        auto src_rect = NSMakeRect(src.left, src.top, src.width(), src.height());
        auto dst_rect = NSMakeRect(dst.left, dst.top, dst.width(), dst.height());

        [img_fr->getNative() drawInRect:dst_rect
                  fromRect:src_rect
                 operation:NSCompositingOperationSourceOver
                  fraction:opacity * opacity_
            respectFlipped:!img_fr->alreadyFilpped()
                     hints:nil];
    }

    void CyroRendererMac::fillOpacityMask(
        float width, float height, ImageFrame* mask, ImageFrame* content)
    {
        auto mask_img = static_cast<const ImageFrameMac*>(mask);
        auto content_img = static_cast<const ImageFrameMac*>(content);

        auto cg_mask = mask_img->getNative().CGImage;
        auto cg_content = content_img->getNative();

        auto context = [[NSGraphicsContext currentContext] CGContext];
        CGContextSaveGState(context);
        CGContextSetAlpha(context, opacity_);
        CGContextClipToMask(context, CGRectMake(0, 0, width, height), cg_mask);

        if (!content_img->alreadyFilpped()) {
            CGContextTranslateCTM(context, 0, cg_content.size.height);
            CGContextScaleCTM(context, 1.0, -1.0);
        }

        CGContextDrawImage(context, CGRectMake(0, 0, width, height), cg_content.CGImage);
        CGContextRestoreGState(context);
    }

    void CyroRendererMac::drawText(
        const std::u16string_view &text,
        const std::u16string_view &font_name, float font_size,
        const RectF &rect, const Paint &paint)
    {
        std::basic_string<unichar> u_fn(font_name.begin(), font_name.end());
        auto ns_font_name = [[NSString alloc] initWithCharacters:
            u_fn.c_str() length:u_fn.length()];
        auto ns_font = [NSFont fontWithName:ns_font_name size:font_size];
        [ns_font_name release];

        std::basic_string<unichar> u_text(text.begin(), text.end());
        auto ns_str =[[NSString alloc] initWithCharacters:
            u_text.c_str() length:u_text.length()];
        auto ns_rect = NSMakeRect(rect.left, rect.top, rect.width(), rect.height());

        NSDictionary* ns_dict = nil;
        if (ns_font) {
            ns_dict = [NSDictionary dictionaryWithObjectsAndKeys:ns_font, NSFontAttributeName, nil];
        }
        [ns_str drawInRect:ns_rect withAttributes:ns_dict];
        [ns_str release];
    }

    void CyroRendererMac::drawTextLayout(
        float x, float y,
        TextLayout *layout, const Paint &paint)
    {
        if (opacity_ < 1.f) {
            auto context = [[NSGraphicsContext currentContext] CGContext];
            CGContextSaveGState(context);
            CGContextSetAlpha(context, opacity_);

            auto mac_layout = static_cast<TextLayoutMac*>(layout);
            mac_layout->nativeDraw(x, y);

            CGContextRestoreGState(context);
        } else {
            auto mac_layout = static_cast<TextLayoutMac*>(layout);
            mac_layout->nativeDraw(x, y);
        }
    }

}
}
