// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/images/image_frame.h"

#include "ukive/app/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/lc_image.h"
#include "ukive/graphics/images/lc_image_factory.h"
#include "ukive/window/window.h"


namespace ukive {

    // static
    ImageFrame* ImageFrame::create(Window *win, const LcImageFrame* frame) {
        return win->getCanvas()->createImage(frame);
    }

    // static
    ImageFrame* ImageFrame::create(Window* win, int width, int height) {
        return win->getCanvas()->createImage(width, height);
    }

    // static
    ImageFrame* ImageFrame::create(
        Window* win, int width, int height, const ImageOptions& options)
    {
        return win->getCanvas()->createImage(width, height, options);
    }

    // static
    ImageFrame* ImageFrame::create(
        Window *win, int width, int height,
        const uint8_t* pixel_data, size_t size, size_t stride)
    {
        return win->getCanvas()->createImage(width, height, pixel_data, size, stride);
    }

    // static
    ImageFrame* ImageFrame::create(
        Window *win, int width, int height,
        const uint8_t* pixel_data, size_t size, size_t stride,
        const ImageOptions& options)
    {
        return win->getCanvas()->createImage(width, height, pixel_data, size, stride, options);
    }

    // static
    ImageFrame* ImageFrame::decodeFile(
        Window* win, const std::u16string& file_name)
    {
        auto ic = Application::getImageLocFactory();
        auto img = ic->decodeFile(
            file_name, win->getCanvas()->getBuffer()->getImageOptions());
        if (!img.isValid()) {
            return nullptr;
        }

        return win->getCanvas()->createImage(img.getFrames()[0]);
    }

    // static
    ImageFrame* ImageFrame::decodeFile(
        Window* win, const std::u16string& file_name, const ImageOptions& options)
    {
        auto ic = Application::getImageLocFactory();
        auto img = ic->decodeFile(file_name, options);
        if (!img.isValid()) {
            return nullptr;
        }

        return win->getCanvas()->createImage(img.getFrames()[0]);
    }

    // static
    ImageFrame* ImageFrame::decodeThumbnail(
        Window *win, const std::u16string& file_name, int width, int height)
    {
        auto ic = Application::getImageLocFactory();

        int real_w, real_h;
        std::string data;
        ImageOptions options;
        if (!ic->getThumbnail(file_name, width, height, &data, &real_w, &real_h, &options)) {
            return {};
        }

        return create(
            win, real_w, real_h,
            reinterpret_cast<const uint8_t*>(data.data()), data.size(), real_w*4, options);
    }

}
