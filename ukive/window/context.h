// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_WINDOW_CONTEXT_H_
#define UKIVE_WINDOW_CONTEXT_H_

#include <memory>

#include "ukive/system/theme_info.h"


namespace ukive {

    class ContextImpl;
    class ContextChangedListener;

    class Context {
    public:
        enum Type {
            NONE,
            DEV_LOST,
            DEV_RESTORE,
            HDR_CHANGED,
            DPI_CHANGED,
            THEME_CHANGED,
            TRANSLUCENT_CHANGED,
        };

        Context();

        void addListener(ContextChangedListener* l);
        void removeListener(ContextChangedListener* l);
        void notifyChanged(Type type);

        void setScale(float s);
        void setAutoScale(float s);
        void setDefaultDpi(uint32_t dpi);
        void setHDREnabled(bool enabled);
        void setTranslucentType(TranslucentType type);

        float getScale() const;
        float getAutoScale() const;
        uint32_t getDefaultDpi() const;
        bool isHDREnabled() const;
        ThemeConfig& getDefaultThemeConfig() const;
        ThemeConfig& getCurrentThemeConfig() const;
        TranslucentType getTranslucentType() const;

        float dp2px(int dp) const;
        float dp2px(float dp) const;
        int dp2pxi(int dp) const;
        int dp2pxi(float dp) const;

        float px2dp(int px) const;
        int px2dpci(int px) const;
        int px2dpfi(int px) const;

        std::shared_ptr<ContextImpl> impl_;
    };

}

#endif  // UKIVE_WINDOW_CONTEXT_H_