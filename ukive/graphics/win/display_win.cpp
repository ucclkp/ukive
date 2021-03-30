// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/display_win.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/system/win/com_ptr.hpp"
#include "ukive/system/win/dynamic_windows_api.h"
#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"
#include "ukive/window/window_dpi_utils.h"
#include "ukive/graphics/win/directx_manager.h"

#include <VersionHelpers.h>
#include <dxgi1_6.h>


namespace {

    const float kDefaultScaleX = 1;
    const float kDefaultScaleY = 1;
    const uint32_t kDefaultRefreshRate = 60;

}

namespace ukive {

    DisplayWin::DisplayWin()
        : monitor_(nullptr),
          monitor_mode_(),
          monitor_info_() {
    }

    bool DisplayWin::makePrimary() {
        POINT pt = { 0, 0 };
        HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return false;
        }

        return queryMonitorInfo(monitor);
    }

    bool DisplayWin::makeFromPoint(const Point& p) {
        POINT pt{ p.x, p.y };
        HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return false;
        }

        return queryMonitorInfo(monitor);
    }

    bool DisplayWin::makeFromRect(const Rect& r) {
        RECT win_rect{ r.left, r.top, r.right, r.bottom };
        HMONITOR monitor = ::MonitorFromRect(&win_rect, MONITOR_DEFAULTTONEAREST);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return false;
        }

        return queryMonitorInfo(monitor);
    }

    bool DisplayWin::makeFromWindow(Window* w) {
        if (!w) {
            return false;
        }

        auto win = static_cast<WindowImplWin*>(w->getImpl());
        if (!win) {
            return false;
        }

        return makeFromWindowImpl(win);
    }

    bool DisplayWin::isInHDRMode() const {
        if (is_empty_ || !cur_output6_) {
            return false;
        }

        DXGI_OUTPUT_DESC1 desc1;
        HRESULT hr = cur_output6_->GetDesc1(&desc1);
        if (FAILED(hr)) {
            return false;
        }

        return desc1.ColorSpace ==
            DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
    }

    void DisplayWin::getName(std::u16string* name) {
        if (is_empty_) {
            name->clear();
            return;
        }

        *name = reinterpret_cast<char16_t*>(monitor_info_.szDevice);
    }

    void DisplayWin::getAdapterName(std::u16string* name) {
        if (is_empty_ || !cur_adapter_) {
            name->clear();
            return;
        }

        DXGI_ADAPTER_DESC adapterDesc;
        HRESULT hr = cur_adapter_->GetDesc(&adapterDesc);
        if (FAILED(hr)) {
            name->clear();
            return;
        }

        *name = reinterpret_cast<char16_t*>(adapterDesc.Description);
    }

    Rect DisplayWin::getBounds() const {
        auto bounds = getPixelBounds();
        if (Application::getOptions().is_auto_dpi_scale) {
            float sx, sy;
            getUserScale(&sx, &sy);
            auto x = bounds.left / sx;
            auto y = bounds.left / sy;
            auto width = bounds.width() / sx;
            auto height = bounds.height() / sy;

            bounds.left = std::floor(x);
            bounds.top = std::floor(y);
            bounds.right = std::ceil(x + width);
            bounds.bottom = std::ceil(y + height);
        }
        return bounds;
    }

    Rect DisplayWin::getWorkArea() const {
        auto bounds = getPixelWorkArea();
        if (Application::getOptions().is_auto_dpi_scale) {
            float sx, sy;
            getUserScale(&sx, &sy);
            auto x = bounds.left / sx;
            auto y = bounds.left / sy;
            auto width = bounds.width() / sx;
            auto height = bounds.height() / sy;

            bounds.left = std::floor(x);
            bounds.top = std::floor(y);
            bounds.right = std::ceil(x + width);
            bounds.bottom = std::ceil(y + height);
        }
        return bounds;
    }

    Rect DisplayWin::getPixelBounds() const {
        if (is_empty_) {
            return Rect(
                0, 0,
                ::GetSystemMetrics(SM_CXSCREEN),
                ::GetSystemMetrics(SM_CYSCREEN));
        }

        auto& rect = monitor_info_.rcMonitor;
        return Rect(
            rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top);
    }

    Rect DisplayWin::getPixelWorkArea() const {
        if (is_empty_) {
            return Rect(
                0, 0,
                ::GetSystemMetrics(SM_CXSCREEN),
                ::GetSystemMetrics(SM_CYSCREEN));
        }

        auto& rect = monitor_info_.rcWork;
        return Rect(
            rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top);
    }

    void DisplayWin::getUserScale(float* sx, float* sy) const {
        if (is_empty_) {
            *sx = kDefaultScaleX;
            *sy = kDefaultScaleY;
            return;
        }

        static bool is_win8_1_or_above = ::IsWindows8Point1OrGreater();
        if (is_win8_1_or_above) {
            UINT dpi_x = USER_DEFAULT_SCREEN_DPI;
            UINT dpi_y = USER_DEFAULT_SCREEN_DPI;
            HRESULT hr = win::UDGetDpiForMonitor(monitor_, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
            if (SUCCEEDED(hr)) {
                *sx = float(dpi_x) / USER_DEFAULT_SCREEN_DPI;
                *sy = float(dpi_y) / USER_DEFAULT_SCREEN_DPI;
                return;
            }
        }

        HDC screen = ::GetDC(nullptr);
        if (!screen) {
            *sx = kDefaultScaleX;
            *sy = kDefaultScaleY;
            return;
        }

        *sx = float(::GetDeviceCaps(screen, LOGPIXELSX)) / USER_DEFAULT_SCREEN_DPI;
        *sy = float(::GetDeviceCaps(screen, LOGPIXELSY)) / USER_DEFAULT_SCREEN_DPI;
        ::ReleaseDC(nullptr, screen);
    }

    uint32_t DisplayWin::getRefreshRate() const {
        if (is_empty_) {
            return kDefaultRefreshRate;
        }
        return monitor_mode_.dmDisplayFrequency;
    }

    bool DisplayWin::makeFromWindowImpl(WindowImplWin* win) {
        HMONITOR monitor;
        if (win->isCreated()) {
            monitor = ::MonitorFromWindow(win->getHandle(), MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                LOG(Log::WARNING) << "Failed to get monitor handle!";
                return false;
            }
        } else {
            auto bounds = win->getBounds();
            RECT win_rect;
            win_rect.left = bounds.left;
            win_rect.top = bounds.top;
            win_rect.right = bounds.right;
            win_rect.bottom = bounds.bottom;
            monitor = ::MonitorFromRect(&win_rect, MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                LOG(Log::WARNING) << "Failed to get monitor handle!";
                return false;
            }
        }

        return queryMonitorInfo(monitor);
    }

    bool DisplayWin::waitForVSync() {
        if (is_empty_ || !cur_output_) {
            return false;
        }

        HRESULT hr = cur_output_->WaitForVBlank();
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to wait vblank: " << hr;
            return false;
        }
        return true;
    }

    bool DisplayWin::queryMonitorInfo(HMONITOR monitor) {
        MONITORINFOEXW info;
        info.cbSize = sizeof(MONITORINFOEXW);

        if (::GetMonitorInfoW(monitor, &info) == 0) {
            LOG(Log::WARNING) << "Failed to get monitor info!";
            return false;
        }

        DEVMODEW mode;
        if (::EnumDisplaySettingsW(
            info.szDevice, ENUM_CURRENT_SETTINGS, &mode) == 0)
        {
            LOG(Log::WARNING) << "Failed to get monitor settings!";
            return false;
        }

        queryDXGIInfo(monitor);

        monitor_ = monitor;
        monitor_info_ = info;
        monitor_mode_ = mode;
        is_empty_ = false;

        return true;
    }

    bool DisplayWin::queryDXGIInfo(HMONITOR monitor) {
        UINT adapter_index = 0;
        for (;;) {
            ComPtr<IDXGIAdapter1> adapter;
            auto dxgi_factory = static_cast<DirectXManager*>(
                Application::getGraphicDeviceManager())->getDXGIFactory();
            HRESULT hr = dxgi_factory->EnumAdapters1(adapter_index, &adapter);
            if (FAILED(hr)) {
                break;
            }

            UINT output_index = 0;
            for (;;) {
                ComPtr<IDXGIOutput> output;
                hr = adapter->EnumOutputs(output_index, &output);
                if (FAILED(hr)) {
                    break;
                }

                DXGI_OUTPUT_DESC out_desc;
                hr = output->GetDesc(&out_desc);
                if (SUCCEEDED(hr)) {
                    if (out_desc.Monitor == monitor) {
                        cur_output_ = output;
                        cur_adapter_ = adapter;
                        cur_output6_ = cur_output_.cast<IDXGIOutput6>();
                        return true;
                    }
                }

                ++output_index;
                output.reset();
            }

            ++adapter_index;
            adapter.reset();
        }

        return false;
    }

}
