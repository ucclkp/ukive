// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FACTORY_WIN_H_
#define UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FACTORY_WIN_H_

#include <wincodec.h>

#include "utils/memory/win/com_ptr.hpp"

#include "ukive/graphics/images/image_data.h"
#include "ukive/graphics/images/lc_image_factory.h"


namespace ukive {
namespace win {

    class LcImageFactoryWin : public LcImageFactory {
    public:
        LcImageFactoryWin();

        bool initialization() override;
        void destroy() override;

        GPtr<LcImageFrame> create(
            int width, int height, const ImageOptions& options) override;
        GPtr<LcImageFrame> create(
            int width, int height,
            void* pixel_data, size_t size, size_t stride,
            const ImageOptions& options) override;

        LcImage decodeFile(
            const std::u16string_view& file_name, const ImageOptions& options) override;
        LcImage decodeMemory(
            const void* buffer, size_t size, const ImageOptions& options) override;

        GPtr<LcImageFrame> getThumbnail(
            const std::u16string_view& file_name,
            int frame_width, int frame_height, ImageOptions* options) override;

        bool saveToFile(
            int width, int height,
            void* data, size_t byte_count, size_t stride,
            ImageContainer container,
            const ImageOptions& options,
            const std::u16string_view& file_name) override;

    private:
        void getGlobalMetadata(
            IWICBitmapDecoder* decoder, GifImageData* data);
        void getFrameMetadata(
            IWICBitmapFrameDecode* decoder, GifImageFrData* data);

        utl::win::ComPtr<IWICBitmapDecoder> createDecoder(
            const std::u16string_view& file_name);
        utl::win::ComPtr<IWICBitmapDecoder> createDecoder(const void* buffer, size_t size);
        utl::win::ComPtr<IWICBitmapSource> convertPixelFormat(
            IWICBitmapSource* frame, const ImageOptions& options);

        LcImage processDecoder(
            IWICBitmapDecoder* decoder, const ImageOptions& options);

        bool exploreColorProfile(IWICBitmapFrameDecode* frame);
        utl::win::ComPtr<IWICBitmapSource> convertGamut(
            IWICBitmapFrameDecode* src, IWICColorContext* dst_cc);

        utl::win::ComPtr<IWICImagingFactory> wic_factory_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FACTORY_WIN_H_