// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_WIN_DYNAMIC_WINDOWS_API_H_
#define UKIVE_SYSTEM_WIN_DYNAMIC_WINDOWS_API_H_

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <Windows.h>
#include <ShellScalingAPI.h>


namespace ukive {
namespace win {

    // Windows 8.1 or later
    HRESULT STDAPICALLTYPE UDGetDpiForMonitor(
        HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

    // Windows 10 1607 or later
    UINT WINAPI UDGetDpiForWindow(HWND hwnd);
    int WINAPI UDGetSystemMetricsForDpi(int nIndex, UINT dpi);

    // Windows 8 or later
    BOOL WINAPI UDSetWindowFeedbackSetting(
        HWND hwnd, FEEDBACK_TYPE feedback, DWORD flags, UINT32 size, const VOID* configuration);

    // Windows 8 or later
    BOOL WINAPI UDGetCurrentInputMessageSource(INPUT_MESSAGE_SOURCE* ims);

    // Windows 8 or later
    STDAPI UDDCompositionCreateDevice(IDXGIDevice *dxgiDevice, REFIID iid, void **dcompositionDevice);

    // 未文档化的结构体和方法，用于显示模糊效果。
    // 这些结构体和函数从网络收集而来，不保证其正确性。
    typedef enum _WINDOWCOMPOSITIONATTRIB
    {
        WCA_UNDEFINED = 0,
        WCA_NCRENDERING_ENABLED = 1,
        WCA_NCRENDERING_POLICY = 2,
        WCA_TRANSITIONS_FORCEDISABLED = 3,
        WCA_ALLOW_NCPAINT = 4,
        WCA_CAPTION_BUTTON_BOUNDS = 5,
        WCA_NONCLIENT_RTL_LAYOUT = 6,
        WCA_FORCE_ICONIC_REPRESENTATION = 7,
        WCA_EXTENDED_FRAME_BOUNDS = 8,
        WCA_HAS_ICONIC_BITMAP = 9,
        WCA_THEME_ATTRIBUTES = 10,
        WCA_NCRENDERING_EXILED = 11,
        WCA_NCADORNMENTINFO = 12,
        WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
        WCA_VIDEO_OVERLAY_ACTIVE = 14,
        WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
        WCA_DISALLOW_PEEK = 16,
        WCA_CLOAK = 17,
        WCA_CLOAKED = 18,
        WCA_ACCENT_POLICY = 19,
        WCA_FREEZE_REPRESENTATION = 20,
        WCA_EVER_UNCLOAKED = 21,
        WCA_VISUAL_OWNER = 22,
        WCA_LAST = 23
    } WINDOWCOMPOSITIONATTRIB;

    typedef struct _WINDOWCOMPOSITIONATTRIBDATA
    {
        WINDOWCOMPOSITIONATTRIB Attrib;
        PVOID pvData;
        SIZE_T cbData;
    } WINDOWCOMPOSITIONATTRIBDATA;

    typedef enum _ACCENT_STATE
    {
        ACCENT_DISABLED = 0,
        ACCENT_ENABLE_GRADIENT = 1,
        ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
        ACCENT_ENABLE_BLURBEHIND = 3,
        ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
        ACCENT_INVALID_STATE = 5
    } ACCENT_STATE;

    typedef struct _ACCENT_POLICY
    {
        ACCENT_STATE AccentState;
        DWORD AccentFlags;
        DWORD GradientColor;
        DWORD AnimationId;
    } ACCENT_POLICY;

    BOOL WINAPI UDSetWindowCompositionAttribute(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA* data);

}
}

#endif  // UKIVE_SYSTEM_WIN_DYNAMIC_WINDOWS_API_H_