// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "display_manager_win.h"

#include "utils/log.h"

#include "ukive/graphics/win/display_win.h"
#include "ukive/window/window.h"
#include "ukive/window/win/window_impl_win.h"


namespace ukive {
namespace win {

    DisplayManagerWin::DisplayManagerWin() {}

    bool DisplayManagerWin::initialize() {
        null_display_ = std::make_shared<DisplayWin>(nullptr);
        enumCCDInfo();
        enumDisplays();
        enumMonitors();
        return true;
    }

    void DisplayManagerWin::destroy() {
        cur_source_ = nullptr;
        ccd_path_info_.reset();
        ccd_monitor_info_.clear();
        dd_list_.clear();
        list_.clear();
        null_display_.reset();
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromNull() {
        return null_display_;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromPrimary() {
        POINT pt = { 0, 0 };
        HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return {};
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromPoint(const Point& p) {
        POINT pt{ p.x(), p.y() };
        HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return {};
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromRect(const Rect& r) {
        RECT win_rect{ r.x(), r.y(), r.right(), r.bottom() };
        HMONITOR monitor = ::MonitorFromRect(&win_rect, MONITOR_DEFAULTTONEAREST);
        if (!monitor) {
            LOG(Log::WARNING) << "Failed to get monitor handle!";
            return {};
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromWindow(const Window* w) {
        if (!w) {
            return {};
        }
        auto win = static_cast<WindowImplWin*>(w->getImpl());
        if (!win) {
            return {};
        }

        return fromWindowImpl(win);
    }

    DisplayManager::DisplayPtr DisplayManagerWin::fromWindowImpl(const WindowImplWin* win) {
        HMONITOR monitor;
        if (win->isCreated()) {
            monitor = ::MonitorFromWindow(win->getHandle(), MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                LOG(Log::WARNING) << "Failed to get monitor handle!";
                return {};
            }
        } else {
            auto bounds = win->getBounds();
            RECT win_rect;
            win_rect.left = bounds.x();
            win_rect.top = bounds.y();
            win_rect.right = bounds.right();
            win_rect.bottom = bounds.bottom();
            monitor = ::MonitorFromRect(&win_rect, MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                LOG(Log::WARNING) << "Failed to get monitor handle!";
                return {};
            }
        }

        auto ptr = findMonitor(monitor);
        if (ptr) {
            return ptr;
        }

        ptr = std::make_shared<DisplayWin>(monitor);
        list_.push_back(ptr);
        return ptr;
    }

    const DisplayManager::DisplayList& DisplayManagerWin::getAllDisplays() const {
        return list_;
    }

    void DisplayManagerWin::notifyChanged(HWND hWnd) {
        if (!hWnd) {
            return;
        }
        if (cur_source_ && cur_source_ != hWnd) {
            return;
        }
        cur_source_ = hWnd;

        enumCCDInfo();
        enumDisplays();
        enumMonitors();

        for (auto l : listeners_) {
            l->onDisplayChanged();
        }
    }

    // static
    BOOL CALLBACK DisplayManagerWin::MonitorEnumProc(
        HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM lParam)
    {
        auto This = reinterpret_cast<DisplayManagerWin*>(lParam);

        auto display = std::make_shared<DisplayWin>(monitor);
        if (display->isValid()) {
            This->list_.push_back(display);
        }

        return TRUE;
    }

    void DisplayManagerWin::enumMonitors() {
        list_.clear();
        BOOL ret = ::EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, LPARAM(this));
        assert(ret != 0);
    }

    void DisplayManagerWin::enumDisplays() {
        dd_list_.clear();

        DWORD adapter_num = 0;
        DISPLAY_DEVICEW adapter_device = {};
        adapter_device.cb = sizeof(DISPLAY_DEVICEW);
        for (;;) {
            BOOL ret = ::EnumDisplayDevicesW(nullptr, adapter_num, &adapter_device, 0);
            if (ret == 0) {
                break;
            }

            ++adapter_num;
            if (!(adapter_device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
                continue;
            }


            DISPLAY_DEVICEW display_device = {};
            display_device.cb = sizeof(DISPLAY_DEVICEW);

            ret = ::EnumDisplayDevicesW(adapter_device.DeviceName, 0, &display_device, 0);
            if (ret != 0) {
                if (!(display_device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
                    continue;
                }
                dd_list_.push_back({ adapter_device, display_device });
            }
        }
    }

    void DisplayManagerWin::enumCCDInfo() {
        ccd_path_info_.reset();
        ccd_monitor_info_.clear();

        // https://docs.microsoft.com/en-us/windows-hardware/drivers/display/connecting-and-configuring-displays
        UINT32 path_count, mode_count;
        auto ret = ::GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &path_count, &mode_count);
        if (ret != 0) {
            return;
        }

        ccd_path_info_.reset(new DISPLAYCONFIG_PATH_INFO[path_count]);
        std::unique_ptr<DISPLAYCONFIG_MODE_INFO[]> modes(new DISPLAYCONFIG_MODE_INFO[mode_count]);
        ret = ::QueryDisplayConfig(
            QDC_ONLY_ACTIVE_PATHS, &path_count, ccd_path_info_.get(), &mode_count, modes.get(), nullptr);
        if (ret != 0) {
            ccd_path_info_.reset();
            return;
        }

        ccd_monitor_info_.resize(path_count);

        for (size_t i = 0; i < path_count; ++i) {
            auto& path = ccd_path_info_.get()[i];
            auto& info = ccd_monitor_info_[i];

            auto& source_info = info.source_info;
            source_info = {};
            source_info.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            source_info.header.size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME);
            source_info.header.adapterId = path.sourceInfo.adapterId;
            source_info.header.id = path.sourceInfo.id;
            ret = ::DisplayConfigGetDeviceInfo(&source_info.header);
            info.has_source_info = (ret == 0);

            auto& target_info = info.target_info;
            target_info = {};
            target_info.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
            target_info.header.size = sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME);
            target_info.header.adapterId = path.targetInfo.adapterId;
            target_info.header.id = path.targetInfo.id;
            ret = ::DisplayConfigGetDeviceInfo(&target_info.header);
            info.has_target_info = (ret == 0);

            auto& color_info = info.advanced_color_info;
            color_info = {};
            color_info.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
            color_info.header.size = sizeof(DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO);
            color_info.header.adapterId = path.targetInfo.adapterId;
            color_info.header.id = path.targetInfo.id;
            ret = ::DisplayConfigGetDeviceInfo(&color_info.header);
            info.has_advanced_color_info = (ret == 0);

            auto& sdr_info = info.sdr_white_level;
            sdr_info = {};
            sdr_info.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
            sdr_info.header.size = sizeof(DISPLAYCONFIG_SDR_WHITE_LEVEL);
            sdr_info.header.adapterId = path.targetInfo.adapterId;
            sdr_info.header.id = path.targetInfo.id;
            ret = ::DisplayConfigGetDeviceInfo(&sdr_info.header);
            info.has_sdr_white_level_info = (ret == 0);
        }
    }

    DisplayManager::DisplayPtr DisplayManagerWin::findMonitor(HMONITOR monitor) const {
        for (auto ptr : list_) {
            if (static_cast<DisplayWin*>(ptr.get())->getNative() == monitor) {
                return ptr;
            }
        }
        return {};
    }

}
}