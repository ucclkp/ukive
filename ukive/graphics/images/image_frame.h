// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_IMAGES_IMAGE_FRAME_H_
#define UKIVE_GRAPHICS_IMAGES_IMAGE_FRAME_H_

#include <string>

#include "ukive/graphics/size.hpp"


namespace ukive {

    class Window;
    class LcImageFrame;
    class ImageOptions;

    class ImageFrame {
    public:
        static ImageFrame* create(Window *win, const LcImageFrame* frame);
        static ImageFrame* create(Window *win, int width, int height);
        static ImageFrame* create(
            Window *win, int width, int height, const ImageOptions& options);
        static ImageFrame* create(
            Window *win, int width, int height,
            const uint8_t* pixel_data, size_t size, size_t stride);
        static ImageFrame* create(
            Window *win, int width, int height,
            const uint8_t* pixel_data, size_t size, size_t stride,
            const ImageOptions& options);

        static ImageFrame* decodeFile(
            Window *win, const std::u16string& file_name);
        static ImageFrame* decodeFile(
            Window *win, const std::u16string& file_name, const ImageOptions& options);
        static ImageFrame* decodeThumbnail(
            Window *win, const std::u16string& file_name, int width, int height);

        virtual ~ImageFrame() = default;

        virtual void getDpi(float* dpi_x, float* dpi_y) const = 0;

        virtual SizeF getSize() const = 0;
        virtual SizeU getPixelSize() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_IMAGES_IMAGE_FRAME_H_
