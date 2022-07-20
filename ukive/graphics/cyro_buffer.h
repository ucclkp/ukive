// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_CYRO_BUFFER_H_
#define UKIVE_GRAPHICS_CYRO_BUFFER_H_

#include <cstddef>

#include "ukive/graphics/gptr.hpp"
#include "ukive/graphics/graphics_utils.h"
#include "ukive/graphics/size.hpp"


namespace ukive {

    class Window;
    class ImageFrame;
    class NativeRT;
    class ImageOptions;

    class CyroBuffer {
    public:
        enum Type {
            WINDOW,
            OFFSCREEN,
        };

        virtual ~CyroBuffer() = default;

        virtual bool onCreate(
            int width, int height,
            const ImageOptions& options) = 0;
        virtual GRet onResize(int width, int height) = 0;
        virtual void onDPIChange(float dpi_x, float dpi_y) = 0;
        virtual void onDestroy() = 0;

        virtual void onBeginDraw() = 0;
        virtual GRet onEndDraw() = 0;

        virtual Size getSize() const = 0;
        virtual Size getPixelSize() const = 0;
        virtual Type getType() const = 0;

        virtual const NativeRT* getNativeRT() const = 0;
        virtual const ImageOptions& getImageOptions() const = 0;

        virtual GPtr<ImageFrame> onExtractImage(const ImageOptions& options) = 0;
    };


    class WindowBuffer : public CyroBuffer {
    public:
        static WindowBuffer* create(Window* w);

        Type getType() const override { return WINDOW; }
    };


    class OffscreenBuffer : public CyroBuffer {
    public:
        static OffscreenBuffer* create();

        Type getType() const override { return OFFSCREEN; }
    };

}

#endif  // UKIVE_GRAPHICS_CYRO_BUFFER_H_
