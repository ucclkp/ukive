// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/context_impl.h"

#include "utils/multi_callbacks.hpp"


namespace ukive {

    ContextImpl::ContextImpl() {
        def_theme_config.type = 0;
        def_theme_config.has_color = true;
        def_theme_config.primary_color = Color::Blue800;
        def_theme_config.primary_color.a = 0.7f;
        def_theme_config.light_theme = true;
        def_theme_config.transparency_enabled = true;

        cur_theme_config = def_theme_config;
    }

    void ContextImpl::addListener(ContextChangedListener* l) {
        utl::addCallbackTo(ccl, l);
    }

    void ContextImpl::removeListener(ContextChangedListener* l) {
        utl::removeCallbackFrom(ccl, l);
    }

    float ContextImpl::dp2px(int dp) const {
        return scale * dp;
    }

    float ContextImpl::dp2px(float dp) const {
        return scale * dp;
    }

    float ContextImpl::px2dp(int px) const {
        return px / scale;
    }

    int ContextImpl::sp2px(int sp) const {
        int mid = static_cast<int>(scale * sp);
        mid += (/*1 -*/ mid % 2);
        return mid;
    }

}
