// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/images/lc_image_factory_mac.h"

#include "ukive/graphics/images/lc_image.h"
#include "ukive/graphics/mac/images/lc_image_frame_mac.h"
#include "ukive/graphics/mac/images/image_options_mac_utils.h"

#import <Cocoa/Cocoa.h>


namespace ukive {
namespace mac {

    bool LcImageFactoryMac::initialization() {
        return true;
    }

    void LcImageFactoryMac::destroy() {
    }

    LcImageFrame* LcImageFactoryMac::create(
        int width, int height, const ImageOptions& options)
    {
        return create(width, height, nullptr, 0, 0, options);
    }

    LcImageFrame* LcImageFactoryMac::create(
         int width, int height,
         void* pixel_data, size_t size, size_t stride,
         const ImageOptions &options)
    {
        auto context = createCGContext(width, height, pixel_data, size, stride, options);
        if (!context) {
            return nullptr;
        }

        auto image = CGBitmapContextCreateImage(context);
        CGContextRelease(context);

        if (!image) {
            return nullptr;
        }

        auto lc_image_fr = new LcImageFrameMac(image);
        switch (options.dpi_type) {
            case ImageDPIType::SPECIFIED:
                lc_image_fr->setDpi(options.dpi_x, options.dpi_y);
                break;
            case ImageDPIType::DEFAULT:
            default:
                break;
        }

        return lc_image_fr;
    }

    LcImage LcImageFactoryMac::decodeFile(
        const std::u16string_view& file_name, const ImageOptions& options)
    {
        std::basic_string<unichar> u_fn(file_name.begin(), file_name.end());
        auto name = [[NSString alloc] initWithCharacters:
                     u_fn.c_str() length:u_fn.length()];

        auto data = CGDataProviderCreateWithFilename(name.UTF8String);
        auto source = CGImageSourceCreateWithDataProvider(data, nullptr);

        LcImage lc_image = processDecoder(source, options);

        CFRelease(source);
        [name release];
        CGDataProviderRelease(data);
        return lc_image;
    }

    LcImage LcImageFactoryMac::decodeMemory(
        void* buffer, size_t size, const ImageOptions& options)
    {
        auto data = [NSData dataWithBytes:buffer length:size];
        CFDataRef cf_data = (CFDataRef)CFBridgingRetain(data);
        auto source = CGImageSourceCreateWithData(cf_data, nullptr);

        LcImage lc_image = processDecoder(source, options);

        CFRelease(source);
        CFRelease(cf_data);
        return lc_image;
    }

    bool LcImageFactoryMac::getThumbnail(
        const std::u16string_view& file_name,
        int flame_width, int frame_height,
        std::string* out, int* real_w, int* real_h, ImageOptions* options)
    {
        return false;
    }

    bool LcImageFactoryMac::saveToFile(
        int width, int height,
        void* data, size_t byte_count, size_t stride,
        ImageContainer container,
        const ImageOptions& options,
        const std::u16string_view& file_name)
    {
        return false;
    }

    LcImage LcImageFactoryMac::processDecoder(CGImageSourceRef source, const ImageOptions& options) {
        LcImage lc_image;
        auto count = CGImageSourceGetCount(source);
        for (decltype(count) i = 0; i < count; ++i) {
            auto image = CGImageSourceCreateImageAtIndex(source, i, nullptr);

            LcImageFrameMac* lc_img_fr;
            if (options.pixel_format == ImagePixelFormat::RAW) {
                lc_img_fr = new LcImageFrameMac(image);
            } else {
                auto new_img = convertPixelFormat(image, options);
                CGImageRelease(image);
                lc_img_fr = new LcImageFrameMac(new_img);
            }

            switch (options.dpi_type) {
                case ImageDPIType::SPECIFIED:
                    lc_img_fr->setDpi(options.dpi_x, options.dpi_y);
                    break;
                case ImageDPIType::DEFAULT:
                default:
                    break;
            }

            lc_image.addFrame(lc_img_fr);
        }
        return lc_image;
    }

    CGContextRef LcImageFactoryMac::createCGContext(
        int width, int height,
        void* pixel_data, size_t size, size_t stride,
        const ImageOptions &options)
    {
        uint32_t info = mapCGBitmapContextInfo(options);
        if (!pixel_data) {
            stride = 0;
        }

        auto color_space = CGColorSpaceCreateDeviceRGB();
        auto context = CGBitmapContextCreate(
                                             pixel_data,
                                             width, height, 8, stride,
                                             color_space, info);
        CGColorSpaceRelease(color_space);
        return context;
    }

    CGImageRef LcImageFactoryMac::convertPixelFormat(CGImageRef img, const ImageOptions& options) {
        auto width = CGImageGetWidth(img);
        auto height = CGImageGetHeight(img);
        auto context = createCGContext(int(width), int(height), nullptr, 0, 0, options);
        if (!context) {
            return nullptr;
        }

        CGContextDrawImage(context, CGRectMake(0, 0, width, height), img);
        auto new_img = CGBitmapContextCreateImage(context);

        CGContextRelease(context);
        return new_img;
    }

}
}
