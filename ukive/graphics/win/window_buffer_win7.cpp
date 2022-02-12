// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/window_buffer_win7.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/system/win/win10_version.h"
#include "ukive/window/win/window_impl_win.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/images/lc_image_frame_win.h"


namespace ukive {
namespace win {

    WindowBufferWin7::WindowBufferWin7(WindowImplWin* w)
        : window_(w),
          rt_(std::make_unique<CyroRenderTargetD2D>()) {}

    bool WindowBufferWin7::onCreate(
        int width, int height, const ImageOptions& options)
    {
        is_layered_ = window_->isLayered();
        img_options_ = options;

        bool ret;
        if (is_layered_) {
            ret = createHardwareBRT();
        } else {
            ret = createSwapchainBRT();
        }
        return ret;
    }

    GRet WindowBufferWin7::onResize(int width, int height) {
        GRet ret;
        if (is_layered_) {
            ret = resizeHardwareBRT() ? GRet::Succeeded : GRet::Failed;
        } else {
            ret = resizeSwapchainBRT();
        }
        return ret;
    }

    void WindowBufferWin7::onDPIChange(float dpi_x, float dpi_y) {
        if (dpi_x <= 0 || dpi_y <= 0) {
            DLOG(Log::ERR) << "Invalid dpi values.";
            return;
        }

        switch (img_options_.dpi_type) {
        case ImageDPIType::SPECIFIED:
            img_options_.dpi_x = dpi_x;
            img_options_.dpi_y = dpi_y;
            rt_->getNative()->SetDpi(dpi_x, dpi_y);
            break;
        default:
            break;
        }
    }

    void WindowBufferWin7::onDestroy() {
        rt_->destroy();
        swapchain_.reset();
    }

    void WindowBufferWin7::onBeginDraw() {
        rt_->getNative()->BeginDraw();
    }

    GRet WindowBufferWin7::onEndDraw() {
        GRet ret = GRet::Succeeded;
        if (is_layered_) {
            ret = drawLayered() ? GRet::Succeeded : GRet::Failed;
        }

        HRESULT hr = rt_->getNative()->EndDraw();
        if (FAILED(hr)) {
            if (hr == D2DERR_RECREATE_TARGET) {
                return GRet::Retry;
            }
            LOG(Log::ERR) << "Failed to draw d2d content.";
            return GRet::Failed;
        }

        if (is_layered_) {
            return ret;
        }

        hr = swapchain_->Present(Application::isVSyncEnabled() ? 1 : 0, 0);
        if (FAILED(hr)) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED ||
                hr == DXGI_ERROR_DEVICE_RESET)
            {
                return GRet::Retry;
            }

            LOG(Log::ERR) << "Failed to present.";
            return GRet::Failed;
        }
        return ret;
    }

    ImageFrame* WindowBufferWin7::onExtractImage(const ImageOptions& options) {
        return nullptr;
    }

    Size WindowBufferWin7::getSize() const {
        if (rt_->getNative()) {
            auto size = rt_->getNative()->GetSize();
            return Size(int(std::ceil(size.width)), int(std::ceil(size.height)));
        }
        return {};
    }

    Size WindowBufferWin7::getPixelSize() const {
        if (rt_->getNative()) {
            auto size = rt_->getNative()->GetPixelSize();
            return Size(size.width, size.height);
        }
        return {};
    }

    CyroRenderTarget* WindowBufferWin7::getRT() const {
        return rt_.get();
    }

    const ImageOptions& WindowBufferWin7::getImageOptions() const {
        return img_options_;
    }

    bool WindowBufferWin7::createHardwareBRT() {
        auto bounds = window_->getBounds();
        auto d3d_texture = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createTexture2D(bounds.width(), bounds.height(), false, false, true);
        if (!d3d_texture) {
            return false;
        }

        auto dxgi_surface = d3d_texture.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to cast tex2d to dxgi surface.";
            return false;
        }

        auto rt = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createDXGIRenderTarget(dxgi_surface.get(), true, img_options_);
        if (!rt) {
            return false;
        }

        rt_->setNative(rt);
        return true;
    }

    bool WindowBufferWin7::createSoftwareBRT() {
        auto bounds = window_->getBounds();
        std::shared_ptr<LcImageFrame> img_loc(
            LcImageFrame::create(bounds.width(), bounds.height(), ImageOptions()));
        if (!img_loc) {
            LOG(Log::WARNING) << "Failed to create wic bitmap.";
            return false;
        }

        auto rt = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createWICRenderTarget(ILF_TO_WIC_BMP(img_loc.get()));
        if (!rt) {
            return false;
        }

        rt_->setNative(rt);
        return true;
    }

    bool WindowBufferWin7::createSwapchainBRT() {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.OutputWindow = window_->getHandle();
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());

        swapchain_.reset();
        HRESULT hr = gdm->getDXGIFactory()->CreateSwapChain(
            gdm->getD3DDevice().get(),
            &swapChainDesc, &swapchain_);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create swap chain: " << hr;
            return false;
        }

        utl::win::ComPtr<IDXGIFactory1> parent;
        hr = swapchain_->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&parent));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to get SwapChain parent: " << hr;
            return false;
        }

        hr = parent->MakeWindowAssociation(window_->getHandle(), DXGI_MWA_NO_ALT_ENTER);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to disable Alt+Enter for SwapChain: " << hr;
            return false;
        }

        utl::win::ComPtr<IDXGISurface> back_buffer;
        hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to query DXGI surface: " << hr;
            return false;
        }

        auto rt = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createDXGIRenderTarget(back_buffer.get(), false, img_options_);
        if (!rt) {
            return false;
        }

        rt_->setNative(rt);
        return true;
    }

    bool WindowBufferWin7::resizeHardwareBRT() {
        if (window_->getBounds().empty()) {
            return true;
        }

        onDestroy();
        return createHardwareBRT();
    }

    bool WindowBufferWin7::resizeSoftwareBRT() {
        if (window_->getBounds().empty()) {
            return true;
        }

        onDestroy();
        return createSoftwareBRT();
    }

    GRet WindowBufferWin7::resizeSwapchainBRT() {
        int width = window_->getTextureWidth();
        int height = window_->getTextureHeight();
        if (width <= 0 || height <= 0) {
            return GRet::Succeeded;
        }

        rt_->destroy();

        HRESULT hr = swapchain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED ||
                hr == DXGI_ERROR_DEVICE_RESET)
            {
                return GRet::Retry;
            }
            LOG(Log::FATAL) << "Failed to resize swap chain: " << hr;
            return GRet::Failed;
        }

        utl::win::ComPtr<IDXGISurface> back_buffer;
        hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to query DXGI surface: " << hr;
            return GRet::Failed;
        }

        auto rt = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createDXGIRenderTarget(back_buffer.get(), false, img_options_);
        if (!rt) {
            return GRet::Failed;
        }

        rt_->setNative(rt);
        return GRet::Succeeded;
    }

    bool WindowBufferWin7::drawLayered() {
        auto gdi_rt = rt_->getNative().cast<ID2D1GdiInteropRenderTarget>();
        if (!gdi_rt) {
            LOG(Log::FATAL) << "Failed to cast ID2D1RenderTarget to GDI RT.";
            return false;
        }

        HDC hdc = nullptr;
        HRESULT hr = gdi_rt->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hdc);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to get DC: " << hr;
            return false;
        }

        RECT wr;
        ::GetWindowRect(window_->getHandle(), &wr);
        POINT zero = { 0, 0 };
        SIZE size = { wr.right - wr.left, wr.bottom - wr.top };
        POINT position = { wr.left, wr.top };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        BOOL ret = ::UpdateLayeredWindow(
            window_->getHandle(),
            nullptr, &position, &size, hdc, &zero,
            RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (ret == 0) {
            LOG(Log::ERR) << "Failed to update layered window: " << ::GetLastError();
        }

        RECT rect = {};
        hr = gdi_rt->ReleaseDC(&rect);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to release DC: " << hr;
            return false;
        }

        return true;
    }

}
}