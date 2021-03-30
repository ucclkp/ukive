// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "ukive/window/context.h"

#include <cmath>

#include "utils/log.h"

#include "ukive/window/context_changed_listener.h"
#include "ukive/window/context_impl.h"


namespace ukive {

    Context::Context()
        : impl_(std::make_shared<ContextImpl>()) {}

    void Context::addListener(ContextChangedListener* l) {
        impl_->addListener(l);
    }

    void Context::removeListener(ContextChangedListener* l) {
        impl_->removeListener(l);
    }

    void Context::notifyChanged(Type type) {
        if (type == NONE) {
            return;
        }

        DCHECK(impl_->changed == NONE);
        impl_->changed = type;

        for (auto l : impl_->ccl) {
            l->onContextChanged(*this);
        }

        impl_->changed = NONE;
    }

    void Context::setScale(float s) {
        impl_->scale = s;
    }

    void Context::setAutoScale(float s) {
        impl_->auto_scale = s;
    }

    void Context::setDefaultDpi(uint32_t dpi) {
        impl_->def_dpi = dpi;
    }

    void Context::setHDREnabled(bool enabled) {
        impl_->hdr_enabled = enabled;
    }

    void Context::setTranslucentType(TranslucentType type) {
        impl_->trans_type = type;
    }

    void Context::setChanged(Type type) {
        impl_->changed = type;
    }

    float Context::getScale() const {
        return impl_->scale;
    }

    float Context::getAutoScale() const {
        return impl_->auto_scale;
    }

    uint32_t Context::getDefaultDpi() const {
        return impl_->def_dpi;
    }

    bool Context::isHDREnabled() const {
        return impl_->hdr_enabled;
    }

    ThemeConfig& Context::getDefaultThemeConfig() const {
        return impl_->def_theme_config;
    }

    ThemeConfig& Context::getCurrentThemeConfig() const {
        return impl_->cur_theme_config;
    }

    TranslucentType Context::getTranslucentType() const {
        return impl_->trans_type;
    }

    Context::Type Context::getChanged() const {
        return impl_->changed;
    }

    float Context::dp2px(int dp) const {
        return impl_->dp2px(dp);
    }

    float Context::dp2px(float dp) const {
        return impl_->dp2px(dp);
    }

    int Context::dp2pxi(int dp) const {
        return int(impl_->dp2px(dp));
    }

    int Context::dp2pxi(float dp) const {
        return int(impl_->dp2px(dp));
    }

    float Context::px2dp(int px) const {
        return impl_->px2dp(px);
    }

    int Context::px2dpci(int px) const {
        return int(std::ceil(impl_->px2dp(px)));
    }

    int Context::px2dpfi(int px) const {
        return int(std::floor(impl_->px2dp(px)));
    }

}
