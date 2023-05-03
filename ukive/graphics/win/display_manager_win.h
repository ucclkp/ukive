// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_DISPLAY_MANAGER_WIN_H_
#define UKIVE_GRAPHICS_WIN_DISPLAY_MANAGER_WIN_H_

#include "ukive/graphics/display_manager.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>


namespace ukive {
namespace win {

    class WindowImplWin;

    class DisplayManagerWin : public DisplayManager {
    public:
        struct CCDisplayInfo {
            bool has_source_info;
            bool has_target_info;
            bool has_advanced_color_info;
            bool has_sdr_white_level_info;
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source_info;
            DISPLAYCONFIG_TARGET_DEVICE_NAME target_info;
            DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO advanced_color_info;
            DISPLAYCONFIG_SDR_WHITE_LEVEL sdr_white_level;
        };

        DisplayManagerWin();

        bool initialize() override;
        void destroy() override;

        DisplayPtr fromNull() override;
        DisplayPtr fromPrimary() override;
        DisplayPtr fromPoint(const Point& p) override;
        DisplayPtr fromRect(const Rect& r) override;
        DisplayPtr fromWindow(const Window* w) override;
        DisplayPtr fromWindowImpl(const WindowImplWin* win);

        const DisplayList& getAllDisplays() const override;

        void notifyChanged(HWND hWnd);
        const std::vector<CCDisplayInfo>& getCCDInfo() const { return ccd_monitor_info_; }

    private:
        struct DisplayDeviceInfo {
            DISPLAY_DEVICEW adapter;
            DISPLAY_DEVICEW display;
        };

        static BOOL CALLBACK MonitorEnumProc(
            HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM lParam);

        void enumMonitors();
        void enumDisplays();
        void enumCCDInfo();
        DisplayPtr findMonitor(HMONITOR monitor) const;

        DisplayPtr null_display_;
        DisplayList list_;
        std::vector<DisplayDeviceInfo> dd_list_;
        std::vector<CCDisplayInfo> ccd_monitor_info_;
        std::unique_ptr<DISPLAYCONFIG_PATH_INFO[]> ccd_path_info_;
        HWND cur_source_ = nullptr;
    };

}
}

#endif  // UKIVE_GRAPHICS_WIN_DISPLAY_MANAGER_WIN_H_