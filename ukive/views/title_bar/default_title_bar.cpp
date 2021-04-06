// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "default_title_bar.h"

#include "ukive/views/text_view.h"
#include "ukive/window/window.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout_info/restraint_layout_info.h"
#include "ukive/views/title_bar/circle_color_button.h"
#include "ukive/elements/color_element.h"
#include "ukive/system/theme_info.h"


namespace ukive {

    DefaultTitleBar::DefaultTitleBar(Context c)
        : DefaultTitleBar(c, {}) {}

    DefaultTitleBar::DefaultTitleBar(Context c, AttrsRef attrs)
        : super(c, attrs),
          title_tv_(nullptr)
    {
        Color color;
        determineColor(&color);

        setShadowRadius(c.dp2pxi(4));
        setBackground(new ColorElement(color));
        initViews();
    }

    DefaultTitleBar::~DefaultTitleBar() {
    }

    void DefaultTitleBar::setColor(const Color& c) {
        auto b = getBackground();
        static_cast<ColorElement*>(b)->setColor(c);
        requestDraw();
    }

    void DefaultTitleBar::setThemeColorEnabled(bool enabled) {
        if (is_theme_color_enabled_ != enabled) {
            is_theme_color_enabled_ = enabled;

            Color color;
            determineColor(&color);
            setColor(color);
        }
    }

    bool DefaultTitleBar::isThemeColorEnabled() const {
        return is_theme_color_enabled_;
    }

    void DefaultTitleBar::initViews() {
        using Rlp = RestraintLayoutInfo;
        auto root_layout = new RestraintLayout(getContext());
        root_layout->setLayoutSize(LS_FILL, LS_FILL);
        addView(root_layout);

        title_tv_ = new TextView(getContext());
        title_tv_->setTextColor(Color::White);
        title_tv_->setTextWeight(TextLayout::FontWeight::BOLD);
        title_tv_->setLayoutMargin(
            getContext().dp2pxi(12), getContext().dp2pxi(12), 0, getContext().dp2pxi(12));
        auto title_tv_lp = Rlp::Builder()
            .start(root_layout->getId())
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        title_tv_->setExtraLayoutInfo(title_tv_lp);
        root_layout->addView(title_tv_);

        // Buttons
        close_btn_ = new CircleColorButton(getContext());
        close_btn_->setOnClickListener(this);
        close_btn_->setColor(Color::Red500);
        close_btn_->setLayoutMargin(0, 0, getContext().dp2pxi(8), 0);
        auto close_btn_lp = Rlp::Builder()
            .end(root_layout->getId())
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        close_btn_->setExtraLayoutInfo(close_btn_lp);
        root_layout->addView(close_btn_);

        max_btn_ = new CircleColorButton(getContext());
        max_btn_->setOnClickListener(this);
        max_btn_->setColor(Color::Yellow500);
        max_btn_->setLayoutMargin(0, 0, getContext().dp2pxi(8), 0);
        auto max_btn_lp = Rlp::Builder()
            .end(close_btn_->getId(), Rlp::START)
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        max_btn_->setExtraLayoutInfo(max_btn_lp);
        root_layout->addView(max_btn_);

        min_btn_ = new CircleColorButton(getContext());
        min_btn_->setOnClickListener(this);
        min_btn_->setColor(Color::Green500);
        min_btn_->setLayoutMargin(0, 0, getContext().dp2pxi(8), 0);
        auto min_btn_lp = Rlp::Builder()
            .end(max_btn_->getId(), Rlp::START)
            .top(root_layout->getId())
            .bottom(root_layout->getId()).build();
        min_btn_->setExtraLayoutInfo(min_btn_lp);
        root_layout->addView(min_btn_);
    }

    void DefaultTitleBar::onClick(View* v) {
        auto w = getWindow();
        if (!w) {
            return;
        }

        if (v == min_btn_) {
            w->minimize();
        } else if (v == max_btn_) {
            if (w->isMaximized()) {
                w->restore();
            } else {
                w->maximize();
            }
        } else if (v == close_btn_) {
            w->close();
        }
    }

    void DefaultTitleBar::onWindowTextChanged(const std::u16string& text) {
        title_tv_->setText(text);
    }

    void DefaultTitleBar::onWindowIconChanged() {
    }

    void DefaultTitleBar::onWindowStatusChanged() {
        if (getWindow()->isMaximized()) {
        } else {
        }
    }

    void DefaultTitleBar::onWindowButtonChanged(WindowButton button) {
        switch (button) {
        case WindowButton::Min:
            min_btn_->setVisibility(
                getWindow()->isMinimizable() ? SHOW : VANISHED);
            break;
        case WindowButton::Max:
            max_btn_->setVisibility(
                getWindow()->isMaximizable() ? SHOW : VANISHED);
            break;
        case WindowButton::Close:
            close_btn_->setVisibility(
                getWindow()->isClosable() ? SHOW : VANISHED);
            break;
        default:
            break;
        }
    }

    void DefaultTitleBar::onAttachedToWindow(Window* w) {
        super::onAttachedToWindow(w);
        title_tv_->setText(getWindow()->getTitle());
    }

    void DefaultTitleBar::onContextChanged(const Context& context) {
        super::onContextChanged(context);

        if (!is_theme_color_enabled_ ||
            context.getChanged() != Context::THEME_CHANGED)
        {
            return;
        }

        auto& config = context.getCurrentThemeConfig();
        switch (config.type) {
        case ThemeConfig::COLOR_CHANGED:
        {
            auto color = config.primary_color;
            if (color.a < 0.3f) color.a = 0.3f;
            setColor(color);
            break;
        }

        case ThemeConfig::COLOR_EXISTANCE:
        {
            Color color;
            determineColor(&color);
            setColor(color);
            break;
        }

        default: break;
        }
    }

    HitPoint DefaultTitleBar::onNCHitTest(int x, int y) {
        if (min_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        if (max_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        if (close_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        return HitPoint::CAPTION;
    }

    void DefaultTitleBar::determineColor(Color* c) {
        if (is_theme_color_enabled_) {
            auto& config = getContext().getCurrentThemeConfig();
            if (config.has_color) {
                *c = config.primary_color;
                if (c->a < 0.3f) c->a = 0.3f;
            } else {
                *c = Color::Blue800;
            }
        } else {
            *c = Color::Blue800;
            c->a = 0.7f;
        }

        //*c = Color(1, 1, 1, 0.5f);
    }

}
