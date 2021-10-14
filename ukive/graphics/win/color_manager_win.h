// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_GRAPHICS_WIN_COLOR_MANAGER_WIN_H_
#define UKIVE_GRAPHICS_WIN_COLOR_MANAGER_WIN_H_

#include "ukive/graphics/color.h"

#include "ukive/graphics/color_manager.h"

#include <Windows.h>
#include <Icm.h>


namespace ukive {

    class ColorManagerWin : public ColorManager {
    public:
        ColorManagerWin();

        bool convertColor(const Color& src, Color* dst) override;

        static bool getICMProfile(HDC hdc, std::wstring* path);
        static bool getSRGBProfile(std::wstring* path);

    private:
        HPROFILE getDisplayProfile();
    };

}

#endif  // UKIVE_GRAPHICS_WIN_COLOR_MANAGER_WIN_H_