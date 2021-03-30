// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/mac/window_buffer_mac.h"

#include "ukive/window/window.h"
#include "ukive/window/mac/window_impl_mac.h"

#import "ukive/window/mac/uk_ns_window.h"


namespace ukive {

    WindowBufferMac::WindowBufferMac(Window* w)
        : win_(w) {}

    bool WindowBufferMac::onCreate(
        size_t width, size_t height, const ImageOptions& options)
    {
        img_options_ = options;
        return true;
    }

    GRet WindowBufferMac::onResize(size_t width, size_t height) {
        return GRet::Succeeded;
    }

    void WindowBufferMac::onDPIChange(float dpi_x, float dpi_y) {
    }

    void WindowBufferMac::onDestroy() {

    }

    void WindowBufferMac::onBeginDraw() {

    }

    GRet WindowBufferMac::onEndDraw() {
        return GRet::Succeeded;
    }

    Size WindowBufferMac::getSize() const {
        auto bounds = win_->getContentBounds();
        return Size(bounds.width(), bounds.height());
    }

    Size WindowBufferMac::getPixelSize() const {
        auto bounds = win_->getContentBounds();
        auto ns_win = static_cast<WindowImplMac*>(win_->getImpl())->getNSWindow();
        float scale = [ns_win backingScaleFactor];
        return Size(int(std::ceil(bounds.width() * scale)), int(std::ceil(bounds.height() * scale)));
    }

    CyroRenderTarget* WindowBufferMac::getRT() const {
        return nullptr;
    }

    const ImageOptions& WindowBufferMac::getImageOptions() const {
        return img_options_;
    }

    ImageFrame* WindowBufferMac::onExtractImage(const ImageOptions& options) {
        return nullptr;
    }

}
