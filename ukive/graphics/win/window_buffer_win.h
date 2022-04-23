// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_WINDOW_BUFFER_WIN_H_
#define UKIVE_GRAPHICS_WIN_WINDOW_BUFFER_WIN_H_

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/native_rt_d2d.h"

#include <dcomp.h>
#include <dxgi1_2.h>


namespace ukive {
namespace win {

    class WindowImplWin;

    class WindowBufferWin : public WindowBuffer
    {
    public:
        explicit WindowBufferWin(WindowImplWin* w);

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
        bool createSoftwareBRT();
        bool createSwapchainBRT();
        bool createDCompositionTree();

        bool resizeSoftwareBRT();
        GRet resizeSwapchainBRT();

        bool use_composition_ = false;
        ImageOptions img_options_;
        WindowImplWin* window_ = nullptr;
        utl::win::ComPtr<IDXGISwapChain1> swapchain_;
        NativeRTD2D nrt_;

        utl::win::ComPtr<IDCompositionDevice> dcomp_dev_;
        utl::win::ComPtr<IDCompositionTarget> dcomp_target_;
        utl::win::ComPtr<IDCompositionVisual> dcomp_visual_;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_WINDOW_BUFFER_WIN_H_