// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_SYSTEM_UI_UTILS_H_
#define UKIVE_SYSTEM_UI_UTILS_H_

#include "ukive/graphics/size.hpp"


namespace ukive {

    /**
     * 返回双击判定时间，单位为毫秒。
     */
    unsigned int getDoubleClickTime();

    /**
     * 返回双击后第三击的判定时间，单位为毫秒。
     */
    unsigned int getTripleClickTime();

    Size getCurrentCursorSize();

}

#endif  // UKIVE_SYSTEM_UI_UTILS_H_