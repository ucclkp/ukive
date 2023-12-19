// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_THEME_INFO_H_
#define UKIVE_SYSTEM_THEME_INFO_H_

#include "ukive/graphics/colors/color.h"


namespace ukive {

    struct ThemeConfig {
        enum Type : uint32_t {
            COLOR_CHANGED   = 1 << 0,
            COLOR_EXISTANCE = 1 << 1,
            CONFIG_CHANGED  = 1 << 2,
        };

        uint32_t type = 0;
        bool has_color;
        Color primary_color;

        bool light_theme;
        bool transparency_enabled;
    };

    /**
     * @brief
     * 窗口的透明样式，具体效果与系统相关。
     * 若使用半透明样式，则窗口上的 UI 元素同样需要具有半透明样式才能看到效果。
     */
    enum TranslucentType {
        /**
         * @brief
         * 系统默认样式，可能根据边框类型而变化。
         * 若 Windows 未激活则强制这种样式，因为其他样式可能出现问题。
         */
        TRANS_DEFAULT = 1 << 0,

        /* 与其他项组合使用，表示跟随系统设置 */
        TRANS_SYSTEM = 1 << 1,

        /* 毛玻璃效果。在 Windows 10 或以上系统具有诸多视觉问题，慎用！*/
        TRANS_BLURBEHIND = 1 << 2,

        /* 全透明。不兼容 Windows 7 非 Aero */
        TRANS_TRANSPARENT = 1 << 3,

        /**
         * @brief
         * 在 Windows 7 上为无硬件加速的全透明窗口，绘制内容的全透明处可穿过鼠标，兼容非 Aero；
         * 其他所有系统与 TRANS_TRANSPARENT 相同。
         */
        TRANS_LAYERED = 1 << 4,

        /* 效果与 TRANS_BLURBEHIND 相同，跟随系统设置 */
        TRANS_BLURBEHIND_SYSTEM = TRANS_BLURBEHIND | TRANS_SYSTEM,

        /* 效果与 TRANS_TRANSPARENT 相同，跟随系统设置 */
        TRANS_TRANSPARENT_SYSTEM = TRANS_TRANSPARENT | TRANS_SYSTEM,
    };

}

#endif  // UKIVE_SYSTEM_THEME_INFO_H_