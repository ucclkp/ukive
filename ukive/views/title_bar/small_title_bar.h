// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UKIVE_VIEWS_TITLE_BAR_SMALL_TITLE_BAR_H_
#define UKIVE_VIEWS_TITLE_BAR_SMALL_TITLE_BAR_H_

#include "ukive/views/click_listener.h"
#include "ukive/views/layout/simple_layout.h"
#include "ukive/views/title_bar/title_bar.h"
#include "ukive/window/window_listener.h"


namespace ukive {

    class Color;
    class Button;
    class TextView;
    class TitleBarButton;

    class SmallTitleBar :
        public TitleBar,
        public OnClickListener
    {
    public:
        explicit SmallTitleBar(Context c);
        SmallTitleBar(Context c, AttrsRef attrs);
        ~SmallTitleBar();

        void setColor(const Color& c);
        void setThemeColorEnabled(bool enabled);

        bool isThemeColorEnabled() const;

        // OnClickListener
        void onClick(View* v) override;

        // OnWindowStatusChangedListener
        void onWindowTextChanged(const std::u16string& text) override;
        void onWindowIconChanged() override;
        void onWindowStatusChanged() override;
        void onWindowButtonChanged(WindowButton button) override;

        void onAttachedToWindow(Window* w) override;
        void onContextChanged(Context::Type type, const Context& context) override;

        HitPoint onNCHitTest(int x, int y) override;

    private:
        using super = TitleBar;

        void initViews();
        void determineColor(Color* c);

        TextView* title_tv_;
        TitleBarButton* min_btn_;
        TitleBarButton* max_btn_;
        TitleBarButton* close_btn_;

        bool is_theme_color_enabled_ = true;
    };

}

#endif  // UKIVE_VIEWS_TITLE_BAR_SMALL_TITLE_BAR_H_