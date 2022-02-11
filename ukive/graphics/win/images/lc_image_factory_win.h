// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FACTORY_WIN_H_
#define UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FACTORY_WIN_H_

#include <wincodec.h>

#include "ukive/graphics/images/image_data.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/system/win/com_ptr.hpp"


namespace ukive {
namespace win {

    class LcImageFactoryWin : public LcImageFactory {
    public:
        LcImageFactoryWin();

        bool initialization() override;
        void destroy() override;

        LcImageFrame* create(
            int width, int height, const ImageOptions& options) override;
        LcImageFrame* create(
            int width, int height,
            uint8_t* pixel_data, size_t size, size_t stride,
            const ImageOptions& options) override;

        LcImage decodeFile(
            const std::u16string& file_name, const ImageOptions& options) override;
        LcImage decodeMemory(
            uint8_t* buffer, size_t size, const ImageOptions& options) override;

        bool getThumbnail(
            const std::u16string& file_name,
            int frame_width, int frame_height,
            std::string* out, int* real_w, int* real_h, ImageOptions* options) override;

        bool saveToFile(
            int width, int height,
            uint8_t* data, size_t byte_count, size_t stride,
            ImageContainer container,
            const ImageOptions& options,
            const std::u16string& file_name) override;

    private:
        void getGlobalMetadata(
            IWICBitmapDecoder* decoder, GifImageData* data);
        void getFrameMetadata(
            IWICBitmapFrameDecode* decoder, GifImageFrData* data);

        ComPtr<IWICBitmapDecoder> createDecoder(const std::u16string& file_name);
        ComPtr<IWICBitmapDecoder> createDecoder(uint8_t* buffer, size_t size);
        ComPtr<IWICBitmapSource> convertPixelFormat(
            IWICBitmapSource* frame, const ImageOptions& options);

        LcImage processDecoder(
            IWICBitmapDecoder* decoder, const ImageOptions& options);

        bool exploreColorProfile(IWICBitmapFrameDecode* frame);
        ComPtr<IWICBitmapSource> convertGamut(
            IWICBitmapFrameDecode* src, IWICColorContext* dst_cc);

        ComPtr<IWICImagingFactory> wic_factory_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_IMAGE_LC_IMAGE_FACTORY_WIN_H_