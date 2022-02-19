// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/text/text_layout.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "ukive/text/win/dw_text_layout.h"
#elif defined OS_MAC
#include "ukive/text/mac/text_layout_mac.h"
#endif


namespace ukive {

    TextLayout* TextLayout::create() {
#ifdef OS_WINDOWS
        return new win::DWTextLayout();
#elif defined OS_MAC
        return new mac::TextLayoutMac();
#endif
    }

}
