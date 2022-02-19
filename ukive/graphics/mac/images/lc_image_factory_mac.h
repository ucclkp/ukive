// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_LC_IMAGE_FACTORY_MAC_H_
#define UKIVE_GRAPHICS_MAC_LC_IMAGE_FACTORY_MAC_H_

#include "ukive/graphics/images/lc_image_factory.h"

#import <CoreGraphics/CoreGraphics.h>
#import <ImageIO/ImageIO.h>


namespace ukive {
namespace mac {

   class LcImageFactoryMac : public LcImageFactory {
   public:
       LcImageFactoryMac() = default;

       bool initialization() override;
       void destroy() override;

       LcImageFrame* create(
            int width, int height, const ImageOptions& options) override;
       LcImageFrame* create(
            int width, int height,
            void* pixel_data, size_t size, size_t stride,
            const ImageOptions &options) override;

       LcImage decodeFile(
            const std::u16string_view& file_name, const ImageOptions& options) override;
       LcImage decodeMemory(
            void* buffer, size_t size, const ImageOptions& options) override;

       bool getThumbnail(
           const std::u16string_view& file_name, int frame_width, int frame_height,
           std::string* out, int* real_w, int* real_h, ImageOptions* options) override;

       bool saveToFile(
           int width, int height,
           void* data, size_t byte_count, size_t stride,
           ImageContainer container,
           const ImageOptions& options,
           const std::u16string_view& file_name) override;

   private:
       LcImage processDecoder(CGImageSourceRef source, const ImageOptions& options);
       CGContextRef createCGContext(
           int width, int height,
           void* pixel_data, size_t size, size_t stride,
           const ImageOptions &options);
       CGImageRef convertPixelFormat(CGImageRef img, const ImageOptions& options);
   };

}
}

#endif  // UKIVE_GRAPHICS_MAC_LC_IMAGE_FACTORY_MAC_H_
