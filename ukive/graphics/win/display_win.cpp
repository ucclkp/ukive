// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/graphics/win/display_win.h"

#include "utils/log.h"

#include "ukive/app/application.h"
#include "ukive/system/win/dynamic_windows_api.h"
#include "ukive/window/win/window_impl_win.h"
#include "ukive/window/window_dpi_utils.h"
#include "ukive/graphics/win/colors/color_manager_win.h"
#include "ukive/graphics/win/display_manager_win.h"

#include <VersionHelpers.h>
#include <dxgi1_6.h>


namespace {

    constexpr float kDefaultScaleX = 1;
    constexpr float kDefaultScaleY = 1;
    constexpr uint32_t kDefaultRefreshRate = 60;

}

namespace ukive {
namespace win {

    // static
    Display::DisplayPtr DisplayWin::fromWindowImpl(const WindowImplWin* win) {
        return static_cast<DisplayManagerWin*>(
            Application::getDisplayManager())->fromWindowImpl(win);
    }

    DisplayWin::DisplayWin(HMONITOR native)
        : monitor_(nullptr)
    {
        if (native) {
            queryMonitorInfo(native);
        }
    }

    DisplayWin::~DisplayWin() {}

    bool DisplayWin::isValid() const {
        return !is_empty_;
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

    bool DisplayWin::isSame(const Display* rhs) const {
        if (!isValid() || !rhs->isValid()) {
            return !isValid() && !rhs->isValid();
        }

        auto rhs_win = static_cast<const DisplayWin*>(rhs);
        if (monitor_ != rhs_win->monitor_) {
            return false;
        }

        if (monitor_mode_.dmBitsPerPel != rhs_win->monitor_mode_.dmBitsPerPel ||
            monitor_mode_.dmPelsWidth != rhs_win->monitor_mode_.dmPelsWidth ||
            monitor_mode_.dmPelsHeight != rhs_win->monitor_mode_.dmPelsHeight ||
            monitor_mode_.dmDisplayFlags != rhs_win->monitor_mode_.dmDisplayFlags ||
            monitor_mode_.dmDisplayFrequency != rhs_win->monitor_mode_.dmDisplayFrequency)
        {
            return false;
        }

        return true;
    }

    bool DisplayWin::isSameDisplay(const Display* rhs) const {
        if (!isValid() || !rhs->isValid()) {
            return !isValid() && !rhs->isValid();
        }

        auto rhs_win = static_cast<const DisplayWin*>(rhs);
        if (monitor_ != rhs_win->monitor_) {
            return false;
        }
        return true;
    }

    void DisplayWin::getName(std::u16string* name) {
        if (is_empty_) {
            name->clear();
            return;
        }

        name->assign(
            monitor_info_.szDevice,
            monitor_info_.szDevice + std::char_traits<WCHAR>::length(monitor_info_.szDevice));
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

        name->assign(
            adapterDesc.Description,
            adapterDesc.Description + std::char_traits<WCHAR>::length(adapterDesc.Description));
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

            bounds.left = int(std::floor(x));
            bounds.top = int(std::floor(y));
            bounds.right = int(std::ceil(x + width));
            bounds.bottom = int(std::ceil(y + height));
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

            bounds.left = int(std::floor(x));
            bounds.top = int(std::floor(y));
            bounds.right = int(std::ceil(x + width));
            bounds.bottom = int(std::ceil(y + height));
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
            UINT dpi_x = kDefaultDpi;
            UINT dpi_y = kDefaultDpi;
            HRESULT hr = win::UDGetDpiForMonitor(monitor_, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
            if (SUCCEEDED(hr)) {
                *sx = float(dpi_x) / kDefaultDpi;
                *sy = float(dpi_y) / kDefaultDpi;
                return;
            }
        }

        HDC screen = ::GetDC(nullptr);
        if (!screen) {
            *sx = kDefaultScaleX;
            *sy = kDefaultScaleY;
            return;
        }

        *sx = float(::GetDeviceCaps(screen, LOGPIXELSX)) / kDefaultDpi;
        *sy = float(::GetDeviceCaps(screen, LOGPIXELSY)) / kDefaultDpi;
        ::ReleaseDC(nullptr, screen);
    }

    uint32_t DisplayWin::getRefreshRate() const {
        if (is_empty_) {
            return kDefaultRefreshRate;
        }
        return monitor_mode_.dmDisplayFrequency;
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

    bool DisplayWin::getICMProfilePath(std::wstring* path) const {
        if (is_empty_) {
            return false;
        }

        HDC hdc = ::CreateDCW(nullptr, monitor_info_.szDevice, nullptr, nullptr);
        if (!hdc) {
            return false;
        }

        bool ret = ColorManagerWin::getICMProfile(hdc, path);

        ::DeleteDC(hdc);
        return ret;
    }

    HMONITOR DisplayWin::getNative() const {
        return monitor_;
    }

    bool DisplayWin::queryMonitorInfo(HMONITOR monitor) {
        MONITORINFOEXW info;
        info.cbSize = sizeof(MONITORINFOEXW);

        if (::GetMonitorInfoW(monitor, &info) == 0) {
            LOG(Log::WARNING) << "Failed to get monitor info!";
            return false;
        }

        DEVMODEW mode;
        mode.dmSize = sizeof(DEVMODEW);
        mode.dmDriverExtra = 0;
        if (::EnumDisplaySettingsW(
            info.szDevice, ENUM_CURRENT_SETTINGS, &mode) == 0)
        {
            LOG(Log::WARNING) << "Failed to get monitor settings!";
            return false;
        }

        queryDXGIInfo(monitor);
        queryCCDInfo(info);

        monitor_ = monitor;
        monitor_info_ = info;
        monitor_mode_ = mode;
        is_empty_ = false;

        return true;
    }

    bool DisplayWin::queryDXGIInfo(HMONITOR monitor) {
        utl::win::ComPtr<IDXGIFactory1> dxgi_factory;
        HRESULT hr = ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi factory: " << hr;
            return false;
        }

        UINT adapter_index = 0;
        for (;;) {
            utl::win::ComPtr<IDXGIAdapter1> adapter;
            hr = dxgi_factory->EnumAdapters1(adapter_index, &adapter);
            if (FAILED(hr)) {
                break;
            }

            UINT output_index = 0;
            for (;;) {
                utl::win::ComPtr<IDXGIOutput> output;
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

    bool DisplayWin::queryCCDInfo(const MONITORINFOEXW& monitor_info) {
        auto& ccd_info = static_cast<DisplayManagerWin*>(
            Application::getDisplayManager())->getCCDInfo();
        for (auto& ccd : ccd_info) {
            if (ccd.has_source_info &&
                std::wcsncmp(monitor_info.szDevice, ccd.source_info.viewGdiDeviceName, CCHDEVICENAME) == 0)
            {
                return true;
            }
        }
        return false;
    }

}
}