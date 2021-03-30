// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/window_buffer_win.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/system/win/win10_version.h"
#include "ukive/system/win/dynamic_windows_api.h"
#include "ukive/window/win/window_impl_win.h"
#include "ukive/graphics/images/image_options.h"
#include "ukive/graphics/win/images/lc_image_frame_win.h"


namespace ukive {

    WindowBufferWin::WindowBufferWin(WindowImplWin* w)
        : window_(w),
          rt_(std::make_unique<CyroRenderTargetD2D>()) {}

    bool WindowBufferWin::onCreate(
        size_t width, size_t height, const ImageOptions& options)
    {
        img_options_ = options;
        return createSwapchainBRT();
    }

    GRet WindowBufferWin::onResize(size_t width, size_t height) {
        return resizeSwapchainBRT();
    }

    void WindowBufferWin::onDPIChange(float dpi_x, float dpi_y) {
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

    void WindowBufferWin::onDestroy() {
        rt_->destroy();
        swapchain_.reset();

        dcomp_visual_.reset();
        dcomp_target_.reset();
        dcomp_dev_.reset();
    }

    void WindowBufferWin::onBeginDraw() {
        rt_->getNative()->BeginDraw();
    }

    GRet WindowBufferWin::onEndDraw() {
        HRESULT hr = rt_->getNative()->EndDraw();
        if (FAILED(hr)) {
            if (hr == D2DERR_RECREATE_TARGET) {
                return GRet::Retry;
            }
            LOG(Log::ERR) << "Failed to draw d2d content.";
            return GRet::Failed;
        }

        DXGI_PRESENT_PARAMETERS params;
        params.DirtyRectsCount = 0;
        params.pDirtyRects = nullptr;
        params.pScrollOffset = nullptr;
        params.pScrollRect = nullptr;
        hr = swapchain_->Present1(Application::isVSyncEnabled() ? 1 : 0, 0, &params);
        if (FAILED(hr)) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED ||
                hr == DXGI_ERROR_DEVICE_RESET)
            {
                return GRet::Retry;
            }

            LOG(Log::ERR) << "Failed to present.";
            return GRet::Failed;
        }
        return GRet::Succeeded;
    }

    ImageFrame* WindowBufferWin::onExtractImage(const ImageOptions& options) {
        return nullptr;
    }

    Size WindowBufferWin::getSize() const {
        if (rt_->getNative()) {
            auto size = rt_->getNative()->GetSize();
            return Size(int(std::ceil(size.width)), int(std::ceil(size.height)));
        }
        return {};
    }

    Size WindowBufferWin::getPixelSize() const {
        if (rt_->getNative()) {
            auto size = rt_->getNative()->GetPixelSize();
            return Size(size.width, size.height);
        }
        return {};
    }

    CyroRenderTarget* WindowBufferWin::getRT() const {
        return rt_.get();
    }

    const ImageOptions& WindowBufferWin::getImageOptions() const {
        return img_options_;
    }

    bool WindowBufferWin::createSoftwareBRT() {
        auto bounds = window_->getBounds();
        if (bounds.empty()) {
            return !!rt_->getNative();
        }

        rt_->destroy();

        std::shared_ptr<LcImageFrame> ilf(
            LcImageFrame::create(bounds.width(), bounds.height(), ImageOptions()));
        if (!ilf) {
            LOG(Log::WARNING) << "Failed to create wic bitmap.";
            return false;
        }

        auto rt = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            createWICRenderTarget(ILF_TO_WIC_BMP(ilf.get()));
        if (!rt) {
            return false;
        }

        rt_->setNative(rt);
        return true;
    }

    bool WindowBufferWin::createSwapchainBRT() {
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());
        auto factory = gdm->getDXGIFactory().cast<IDXGIFactory2>();
        if (!factory) {
            return false;
        }

        swapchain_.reset();
        auto hwnd = window_->getHandle();

        DXGI_SWAP_CHAIN_DESC1 desc;
        ZeroMemory(&desc, sizeof(desc));

        // https://docs.microsoft.com/en-us/windows/win32/direct3darticles/high-dynamic-range
        if (img_options_.pixel_format == ImagePixelFormat::HDR) {
            desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        } else {
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        }

        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

        HRESULT hr;
        if (!use_composition_) {
            if (img_options_.pixel_format == ImagePixelFormat::HDR) {
                desc.BufferCount = 2;
                desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            } else {
                desc.BufferCount = 1;
                desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
            }
            desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

            hr = factory->CreateSwapChainForHwnd(
                gdm->getD3DDevice().get(),
                hwnd, &desc, nullptr, nullptr, &swapchain_);
            if (FAILED(hr)) {
                LOG(Log::FATAL) << "Failed to create swap chain: " << hr;
                return false;
            }
        } else {
            desc.BufferCount = 2;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
            desc.Width = window_->getTextureWidth();
            desc.Height = window_->getTextureHeight();

            hr = factory->CreateSwapChainForComposition(
                gdm->getD3DDevice().get(), &desc, nullptr, &swapchain_);
            if (FAILED(hr)) {
                LOG(Log::FATAL) << "Failed to create swap chain: " << hr;
                return false;
            }

            if (!createDCompositionTree()) {
                return false;
            }
        }

        ComPtr<IDXGIFactory2> parent;
        hr = swapchain_->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&parent));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to get SwapChain parent: " << hr;
            return false;
        }

        hr = parent->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to disable Alt+Enter for SwapChain: " << hr;
            return false;
        }

        ComPtr<IDXGISurface> back_buffer;
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

    bool WindowBufferWin::createDCompositionTree() {
        dcomp_dev_.reset();
        dcomp_target_.reset();
        dcomp_visual_.reset();
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());

        HRESULT hr = win::UDDCompositionCreateDevice(
            gdm->getDXGIDevice().get(),
            __uuidof(IDCompositionDevice),
            reinterpret_cast<void**>(&dcomp_dev_));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create DComposition device: " << hr;
            return false;
        }

        hr = dcomp_dev_->CreateTargetForHwnd(
            window_->getHandle(), FALSE, &dcomp_target_);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create DComposition target: " << hr;
            return false;
        }

        hr = dcomp_dev_->CreateVisual(&dcomp_visual_);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create DComposition visual: " << hr;
            return false;
        }

        hr = dcomp_visual_->SetContent(swapchain_.get());
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to set content of visual: " << hr;
            return false;
        }

        hr = dcomp_target_->SetRoot(dcomp_visual_.get());
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to set root of target: " << hr;
            return false;
        }

        hr = dcomp_dev_->Commit();
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to commit visual tree: " << hr;
            return false;
        }
        return true;
    }

    bool WindowBufferWin::resizeSoftwareBRT() {
        if (window_->getBounds().empty()) {
            return true;
        }

        rt_->destroy();
        return createSoftwareBRT();
    }

    GRet WindowBufferWin::resizeSwapchainBRT() {
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

        ComPtr<IDXGISurface> back_buffer;
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

}
