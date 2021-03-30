// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_CONTEXT_IMPL_H_
#define UKIVE_WINDOW_CONTEXT_IMPL_H_

#include <vector>

#include "ukive/window/context.h"
#include "ukive/system/theme_info.h"


namespace ukive {

    class ContextChangedListener;

    class ContextImpl {
    public:
        ContextImpl();

        void addListener(ContextChangedListener* l);
        void removeListener(ContextChangedListener* l);

        float dp2px(int dp) const;
        float dp2px(float dp) const;
        float px2dp(int px) const;

        Context::Type changed = Context::NONE;

        float scale = 1.f;
        float auto_scale = 1.f;
        uint32_t def_dpi = 96;
        bool hdr_enabled = false;

        ThemeConfig def_theme_config;
        ThemeConfig cur_theme_config;
        TranslucentType trans_type = TRANS_OPAQUE;

        std::vector<ContextChangedListener*> ccl;
    };

}

#endif  // UKIVE_WINDOW_CONTEXT_IMPL_H_