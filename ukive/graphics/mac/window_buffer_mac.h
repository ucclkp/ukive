// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_MAC_WINDOW_BUFFER_MAC_H_
#define UKIVE_GRAPHICS_MAC_WINDOW_BUFFER_MAC_H_

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_options.h"


namespace ukive {

    class Window;

namespace mac {

    class WindowBufferMac : public WindowBuffer {
    public:
        explicit WindowBufferMac(Window* w);

        bool onCreate(
            int width, int height, const ImageOptions& options) override;
        GRet onResize(int width, int height) override;
        void onDPIChange(float dpi_x, float dpi_y) override;
        void onDestroy() override;

        void onBeginDraw() override;
        GRet onEndDraw() override;

        Size getSize() const override;
        Size getPixelSize() const override;

        CyroRenderTarget* getRT() const override;
        const ImageOptions& getImageOptions() const override;

        ImageFrame* onExtractImage(const ImageOptions& options) override;

    private:
        Window* win_;
        ImageOptions img_options_;
    };

}
}

#endif  // UKIVE_GRAPHICS_MAC_WINDOW_BUFFER_MAC_H_
