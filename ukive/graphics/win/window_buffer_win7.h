// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_WINDOW_BUFFER_WIN7_H_
#define UKIVE_GRAPHICS_WIN_WINDOW_BUFFER_WIN7_H_

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/native_rt_d2d.h"


namespace ukive {
namespace win {

    class WindowImplWin;

    class WindowBufferWin7 : public WindowBuffer {
    public:
        explicit WindowBufferWin7(WindowImplWin* w);

        bool onCreate(
            int width, int height, const ImageOptions& options) override;
        bool onRecreate() override;
        GRet onResize(int width, int height) override;
        void onDPIChange(float dpi_x, float dpi_y) override;
        void onDestroy() override;

        void onBeginDraw() override;
        GRet onEndDraw() override;

        GPtr<ImageFrame> onExtractImage(const ImageOptions& options) override;

        Size getSize() const override;
        Size getPixelSize() const override;

        const NativeRT* getNativeRT() const override;
        const ImageOptions& getImageOptions() const override;

    private:
        bool createHardwareBRT();
        bool createSoftwareBRT();
        bool createSwapchainBRT();

        bool resizeHardwareBRT();
        bool resizeSoftwareBRT();
        GRet resizeSwapchainBRT();

        bool drawLayered();

        bool is_layered_ = false;
        ImageOptions img_options_;
        WindowImplWin* window_ = nullptr;
        NativeRTD2D nrt_;
        utl::win::ComPtr<IDXGISwapChain> swapchain_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_WINDOW_BUFFER_WIN7_H_