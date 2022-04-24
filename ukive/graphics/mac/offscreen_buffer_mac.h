// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_OFFSCREEN_BUFFER_MAC_H_
#define UKIVE_GRAPHICS_MAC_OFFSCREEN_BUFFER_MAC_H_

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_options.h"

#include "utils/mac/objc_utils.hpp"

#import <CoreGraphics/CoreGraphics.h>

UTL_OBJC_CLASS(NSGraphicsContext);


namespace ukive {
namespace mac {

    class OffscreenBufferMac : public OffscreenBuffer {
    public:
        OffscreenBufferMac();

        bool onCreate(
            int width, int height, const ImageOptions& options) override;
        bool onRecreate() override;
        GRet onResize(int width, int height) override;
        void onDPIChange(float dpi_x, float dpi_y) override;
        void onDestroy() override;

        void onBeginDraw() override;
        GRet onEndDraw() override;

        Size getSize() const override;
        Size getPixelSize() const override;

        const NativeRT* getNativeRT() const override;
        const ImageOptions& getImageOptions() const override;

        GPtr<ImageFrame> onExtractImage(const ImageOptions& options) override;

    private:
        bool createBuffer();

        int width_, height_;
        NSGraphicsContext* prev_context_ = nullptr;
        CGContextRef cg_context_ = nullptr;
        ImageOptions img_options_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_OFFSCREEN_BUFFER_MAC_H_
