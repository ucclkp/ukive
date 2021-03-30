// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ui_utils_win.h"

#include <dwmapi.h>
#include <ShellScalingAPI.h>
#include <VersionHelpers.h>

#include "ukive/system/win/dynamic_windows_api.h"


namespace ukive {
namespace win {

    bool isAeroEnabled() {
        BOOL dwm_enabled = FALSE;
        HRESULT hr = ::DwmIsCompositionEnabled(&dwm_enabled);
        return (SUCCEEDED(hr) && dwm_enabled == TRUE);
    }

}

    unsigned int getDoubleClickTime() {
        return ::GetDoubleClickTime();
    }

}
